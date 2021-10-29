#include "Engine/pch.h"
#include "HShaderBase.h"
#include "GL/glew.h"
#include "Engine/Render/RenderUtil/GLUtil.h"
#include <regex>


const DSet<DString8> HShaderBase::EMPTY_UNIFORM_NAME_SET;


HShaderBase::~HShaderBase() 
{
    this->FreeGPU();
}


void HShaderBase::FreeGPU()
{
    if (this->ProgramId != 0)
    {
        glCheck(glDeleteProgram(this->ProgramId));
    }
    this->ProgramId = 0;
}


void HShaderBase::DeleteShaders(const DArray<uint32>& ShaderIds)
{
    for (uint32 id : ShaderIds)
    {
        glCheck(glDeleteShader(id));
    }
}


void HShaderBase::_Load(const DArray<DString>& SourceFilepaths)
{
    this->Uniforms.Clear();

    this->ProgramId = glCreateProgram();

    DArray<GLuint> shaderIds;
    shaderIds.Reserve(SourceFilepaths.Length());

    const DSet<DString8>& baseUniformNames = this->GetBaseUniformNames();

    for (const DString& sourceFilepath : SourceFilepaths)
    {
        DString8 sourceCode;
        if (STextFile::ReadAsciiFile(sourceFilepath, sourceCode))
        {
            // --------------- FIND OUT SHADER TYPE ---------------

            DString extension = SPath::GetExtension(sourceFilepath);
            GLenum glShaderType;
            if (extension == TX("vert")) glShaderType = GL_VERTEX_SHADER;
            else if (extension == TX("frag")) glShaderType = GL_FRAGMENT_SHADER;
            else if (extension == TX("geom")) glShaderType = GL_GEOMETRY_SHADER;
            else if (extension == TX("comp")) glShaderType = GL_COMPUTE_SHADER;
            else if (extension == TX("tesc")) glShaderType = GL_TESS_CONTROL_SHADER;
            else if (extension == TX("tese")) glShaderType = GL_TESS_EVALUATION_SHADER;
            else
            {
                glShaderType = GL_VERTEX_SHADER;
                SLog::Error(TX("Can't determine shader type based on extension: ") + sourceFilepath);
                DeleteShaders(shaderIds);
                return;
            }

            glCheck(GLuint shaderId = glCreateShader(glShaderType));
            shaderIds.Add(shaderId);

            // ----------- INCLUDES -----------

            /*
            limitations:
            - include statements in multi-line comments can get resolved
            - cyclic dependency is not detected!!!
            */

            std::basic_regex<char8, std::regex_traits<char8>> pattern ("^\\s*#include \"(.*)\"");
            std::match_results<const char8*> groups;
            const char8* start = sourceCode.begin();
            const char8* end = sourceCode.end();

            while (std::regex_search(start, end, groups, pattern))
            {
                if (groups.size() >= 1)
                {
                    DStringView8 asciiView (groups[1].first, (int32)groups[1].length());
                    DString includedFilepath = DStrings::AsciiToUTF16(asciiView);
                    DString includedDirpath = SPath::GetDirectory(sourceFilepath);
                    if (includedDirpath.Length() > 0)
                    {
                        DString includedResolvedPath = includedDirpath + includedFilepath;
                        DString8 includedSourceCode;
                        if (STextFile::ReadAsciiFile(includedResolvedPath, includedSourceCode))
                        {
                            sourceCode.ReplaceRange((int32)groups.position(), (int32)groups.length(), includedSourceCode);
                        }
                        else
                        {
                            SLog::Error(TX("Can't include code from \"") + includedFilepath + TX("\" to \"") + sourceFilepath + TX("\""));
                            DeleteShaders(shaderIds);
                            return;
                        }

                        // if the included code has includes, they will be included as well
                        start = sourceCode.begin() + groups.position();
                        end = sourceCode.end();
                    }
                    else
                    {
                        SLog::Error(TX("Can't include code from \"") + includedFilepath + TX("\" to \"") + sourceFilepath + TX("\""));
                        DeleteShaders(shaderIds);
                        return;
                    }
                }
            }

            // --------------- COMPILE SHADER ---------------

            const char* src = sourceCode.CString();
            glCheck(glShaderSource(shaderId, 1, &src, nullptr));
            glCheck(glCompileShader(shaderId));

            int32 result;
            glCheck(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result));
            if (result == GL_TRUE)
            {
                // ----------- ATTACH SHADER TO PROGRAM -----------

                glCheck(glAttachShader(this->ProgramId, shaderId));

                // --------------- PARSE UNIFORMS -----------------

                // <newline>[whitespace]uniform name type
                // not perfect, would parse uniform declarations in multiline comments
                std::basic_regex<char8, std::regex_traits<char8>> pattern("^\\s*uniform (\\w+) (\\w+)");
                std::match_results<const char8*> groups;
                const char8* start = sourceCode.begin();
                const char8* end = sourceCode.end();

                while (std::regex_search(start, end, groups, pattern))
                {
                    if (groups.size() >= 2)
                    {
                        start = groups.suffix().first;
                        auto& g1 = groups[1];
                        auto& g2 = groups[2];
                        DString8 uniformType (g1.first, (int32)g1.length());
                        DString8 uniformNameAscii (g2.first, (int32)g2.length());
                        DString uniformName = DStrings::AsciiToUTF16(uniformNameAscii);
                        if (baseUniformNames.Contains(uniformNameAscii) == false)
                        {
                            if (uniformType == "float")
                            {
                                DShaderUniformInfo& info = this->Uniforms.AddDefault(uniformName);
                                info.Type = EShaderParamType::FLOAT;
                            }
                            else if (uniformType == "vec4")
                            {
                                DShaderUniformInfo& info = this->Uniforms.AddDefault(uniformName);
                                info.Type = EShaderParamType::VEC4;
                            }
                            else if (uniformType == "sampler2D")
                            {
                                DShaderUniformInfo& info = this->Uniforms.AddDefault(uniformName);
                                info.Type = EShaderParamType::TEXTURE;
                            }
                            else if (uniformType == "samplerCube")
                            {
                                DShaderUniformInfo& info = this->Uniforms.AddDefault(uniformName);
                                info.Type = EShaderParamType::TEXTURE;
                            }
                            else if (uniformType == "sampler2DMS")
                            {
                                DShaderUniformInfo& info = this->Uniforms.AddDefault(uniformName);
                                info.Type = EShaderParamType::TEXTURE;
                            }
                            else if (uniformType == "int" || uniformType == "bool")
                            {
                                DShaderUniformInfo& info = this->Uniforms.AddDefault(uniformName);
                                info.Type = EShaderParamType::INT;
                            }
                        }
                    }
                } 
            }
            else
            {
                int32 length;
                glCheck(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
                DString8 message;
                message.Resize(length);
                glCheck(glGetShaderInfoLog(shaderId, length, &length, message.GetDataUnsafe()));

                DString shaderTypeStr;
                switch (glShaderType)
                {
                case GL_VERTEX_SHADER: shaderTypeStr = TX("vertex"); break;
                case GL_FRAGMENT_SHADER: shaderTypeStr = TX("fragment"); break;
                case GL_GEOMETRY_SHADER: shaderTypeStr = TX("geometry"); break;
                case GL_COMPUTE_SHADER: shaderTypeStr = TX("compute"); break;
                case GL_TESS_CONTROL_SHADER: shaderTypeStr = TX("tessellation control"); break;
                case GL_TESS_EVALUATION_SHADER: shaderTypeStr = TX("tessellation evaluation"); break;
                default: shaderTypeStr = TX("whatever");
                }

                SLog::Error(
                    DString(TX("Failed to compile ")) + shaderTypeStr + TX(" shader: ") + sourceFilepath
                );
                SLog::Error(message);
                DeleteShaders(shaderIds);
                return;
            }
        }
        else
        {
            SLog::Error(TX("Failed to load file: ") + this->Filepath);
            DeleteShaders(shaderIds);
            return;
        }
    }

    // ----------- LINK PROGRAM ------------

    glCheck(glLinkProgram(this->ProgramId));
    glCheck(glValidateProgram(this->ProgramId));

    // ---------- GET UNIFORM LOCATIONS --------------

    glCheck(glUseProgram(this->ProgramId));

    int32 textureUnit = 0;
    for (auto& entry : this->Uniforms)
    {
        const DString& uniformName = entry.GetKey();
        DShaderUniformInfo& uniform = entry.GetValue();
        uniform.Location = this->GetUniformLocFromGPU(DStrings::UTF16ToAscii(uniformName));
        if (uniform.Location != GL_INVALID_INDEX)
        {
            switch (uniform.Type)
            {
            case EShaderParamType::FLOAT:
            {
                float value = 0;
                glGetUniformfv(this->ProgramId, uniform.Location, &value);
                uniform.DefaultValue = FVec4(value, .0f, .0f, .0f);
                break;
            }
            case EShaderParamType::VEC4:
            {
                float arr[4] { .0f, .0f, .0f, .0f };
                glGetUniformfv(this->ProgramId, uniform.Location, arr);
                uniform.DefaultValue = FVec4(arr[0], arr[1], arr[2], arr[3]);
                break;
            }
            case EShaderParamType::TEXTURE:
            {
                uniform.TextureUnit = textureUnit;
                this->AssignSamplerToTextureUnit(uniform.Location, textureUnit);
                textureUnit++;
                break;
            }
            case EShaderParamType::INT:
            {
                int32 value = 0;
                glGetUniformiv(this->ProgramId, uniform.Location, &value);
                uniform.DefaultValue = FVec4((float)value, .0f, .0f, .0f);
                break;
            }
            default:
            {
                SLog::Error("Unhandled ShaderParamType");
            }
            }
        }
        else
        {
            SLog::Error(TX("Uniform \"") + uniformName + TX("\" not found in shader \"") + this->Name + TX("\""));
        }
    }

    this->TextureUnitCount = textureUnit;

    // ----------- DELETE SHADERS -------------
    
    DeleteShaders(shaderIds);
    
    // ------ EXTRA INITIALIZATION IN SUBCLASS -------

    this->OnLoad();

    glCheck(glUseProgram(0));
}


void HShaderBase::OnLoad()
{
}


bool HShaderBase::IsLoaded()
{
    return this->ProgramId != 0;
}


void HShaderBase::Unload()
{
    this->FreeGPU();
    this->Uniforms.Clear();
}


void HShaderBase::Use()
{
    if (this->ProgramId != 0)
    {
        glCheck(glUseProgram(this->ProgramId));
    }
}


void HShaderBase::UseNone()
{
    glCheck(glUseProgram(0));
}


uint32 HShaderBase::glGetProgramId()
{
    return this->ProgramId;
}


const DMap<DString, DShaderUniformInfo>& HShaderBase::GetUniforms()
{
    return this->Uniforms;
}


const DSet<DString8>& HShaderBase::GetBaseUniformNames()
{
    return EMPTY_UNIFORM_NAME_SET;
}


int32 HShaderBase::GetUniformLoc(const DString& InName)
{
    DShaderUniformInfo* info = this->Uniforms.Find(InName);
    if (info)
    {
        return info->Location;
    }
    else
    {
        SLog::Error(TX("Uniform \"") + InName + TX("\" not found in shader \"") + this->Name + TX("\""));
        return -1;
    }
}


int32 HShaderBase::GetTextureUnit(const DString& InName)
{
    DShaderUniformInfo* info = this->Uniforms.Find(InName);
    if (info)
    {
        return info->TextureUnit;
    }
    else
    {
        SLog::Error(TX("Uniform \"") + InName + TX("\" not found in shader \"") + this->Name + TX("\""));
        return -1;
    }
}


int32 HShaderBase::GetUniformLocFromGPU(DStringView8 InName)
{
    glCheck(return glGetUniformLocation(this->ProgramId, InName.Data()));
}


void HShaderBase::AssignSamplerToTextureUnit(int32 Location, int32 TextureUnitIndex)
{
    glCheck(glUniform1i(Location, TextureUnitIndex));
}


void HShaderBase::SetBoolParam(int32 Location, bool Value)
{
    glCheck(glUniform1i(Location, Value));
}


void HShaderBase::SetFloatParam(int32 Location, float Value)
{
    glCheck(glUniform1f(Location, Value));
}


void HShaderBase::SetIntParam(int32 Location, int32 Value)
{
    glCheck(glUniform1i(Location, Value));
}


void HShaderBase::SetVec2Param(int32 Location, FVec2 Vec)
{
    glCheck(glUniform2f(Location, Vec.X, Vec.Y));
}


void HShaderBase::SetVec2Param(int32 Location, float X, float Y)
{
    glCheck(glUniform2f(Location, X, Y));
}


void HShaderBase::SetVec3Param(int32 Location, const FVec3& Vec)
{
    glCheck(glUniform3f(Location, Vec.X, Vec.Y, Vec.Z));
}


void HShaderBase::SetVec4Param(int32 Location, const FVec3& Vec, float W)
{
    glCheck(glUniform4f(Location, Vec.X, Vec.Y, Vec.Z, W));
}


void HShaderBase::SetVec4Param(int32 Location, const FVec4& Vec)
{
    glCheck(glUniform4fv(Location, 1, &Vec.X));
}


void HShaderBase::SetMat4Param(int32 Location, const FMat4& Mat)
{
    glCheck(glUniformMatrix4fv(Location, 1, GL_FALSE, &Mat.Data[0][0]));
}


void HShaderBase::SetTextureParam(int32 TextureUnit, HTextureBase* InTexture)
{
    glCheck(glActiveTexture(GL_TEXTURE0 + TextureUnit));
    if (InTexture)
    {
        InTexture->Bind();
    }
    else
    {
        HTextureBase::BindNoneStatic();
    }
}


void HShaderBase::SetFloatArrayParam(int32 Location, const float* Array, int32 Length)
{
    glCheck(glUniform1fv(Location, Length, &Array[0]));
}


void HShaderBase::SetIntArrayParam(int32 Location, const int32* Array, int32 Length)
{
    glCheck(glUniform1iv(Location, Length, &Array[0]));
}


void HShaderBase::SetVec2ArrayParam(int32 Location, const FVec2* Array, int32 Length)
{
    glCheck(glUniform2fv(Location, Length, &Array[0].X));
}


void HShaderBase::SetVec3ArrayParam(int32 Location, const FVec3* Array, int32 Length)
{
    glCheck(glUniform3fv(Location, Length, &Array[0].X));
}


void HShaderBase::SetVec4ArrayParam(int32 Location, const FVec4* Array, int32 Length)
{
    glCheck(glUniform4fv(Location, Length, &Array[0].X));
}


void HShaderBase::SetMat4ArrayParam(int32 Location, const FMat4* Array, int32 Length)
{
    glCheck(glUniformMatrix4fv(Location, Length, GL_FALSE, &(*Array).Data[0][0]));
}
