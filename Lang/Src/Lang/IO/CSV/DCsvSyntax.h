#pragma once


struct DCsvSyntax
{
    char8 Delimiter = ',';
    char8 EscapeChar = '\"';

    DString8 FalseStr = "false";
    DString8 TrueStr = "true";
    DString8 ExportKeyName = "Name";
    bool ExportQuoteAllFields = false;
};