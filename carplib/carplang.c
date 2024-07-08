#include "carplang.h"

#include "carpassert.h"

#include <stdlib.h> // calloc
#include <string.h> // memcpy

typedef enum TokenType
{
    TokenTypeUnknownToken,
    TokenTypeEndOfTokens, // '\0'
    TokenTypePercentSign, // %
    TokenTypePlus, // +
    TokenTypeMinus, // -
    TokenTypeStar, // *
    TokenTypeSlash, // /
    TokenTypeBackSlash, // '\'
    TokenTypeComma, // ,
    TokenTypeDot, // .
    TokenTypeF, // 'f'
    TokenTypeSetEqual, // :=
    TokenTypeNotEqual, // !=
    TokenTypeDoubleEqual, // ==
    TokenTypeDoubleQuote, // "
    TokenTypeExclamationMark, // !
    TokenTypeAtSign, // @
    TokenTypeBitwiseAnd, // &
    TokenTypeBitwiseOr, // |
    TokenTypeLogicalAnd, // ||
    TokenTypeLogicalOr, // ||
    TokenTypeHashSign, // #
    TokenTypeQuestionMark, // ?
    TokenTypeColon, // :
    TokenTypeSemiColon, // ;
    TokenTypeParenthesesOpen, // ()
    TokenTypeParenthesesClose,
    TokenTypeSquareBracketOpen, // []
    TokenTypeSquareBracketClose,
    TokenTypeCurlyBracketOpen, // {}
    TokenTypeCurlyBracketClose,
    TokenTypeAngleBracketOpen, // <>
    TokenTypeAngleBracketClose,

    // Number type
    TokenTypeNumber,

    // Keywords
    TokenTypeConst,
    TokenTypeWhile,
    TokenTypeFunc,
    TokenTypeLet,
    TokenTypeVar,

    TokenTypeIdentifier,

    TokenTypeCount,
} TokenType;


typedef struct Token
{
    s32 tokenStart;
    s32 tokenLen;
    TokenType tokenType;
} Token;


typedef struct TokenHelper
{
    const char* tokenHelperStr;
    s32 tokenHelperLen;
    TokenType tokenHelperType;
} TokenHelper;

#define TokenHelperLine(name, type) { name, (sizeof(name) - 1), type }

static const TokenHelper tokenHelpers[] =
{
    TokenHelperLine("\0", TokenTypeEndOfTokens), // '\0'
    TokenHelperLine(":=", TokenTypeSetEqual), // :=
    TokenHelperLine("!=", TokenTypeNotEqual), // !=
    TokenHelperLine("==", TokenTypeDoubleEqual), // ==
    TokenHelperLine("\"", TokenTypeDoubleQuote), // "
    TokenHelperLine("\\", TokenTypeBackSlash), // '\'
    TokenHelperLine("||", TokenTypeLogicalOr), // '\'
    TokenHelperLine("&&", TokenTypeLogicalAnd), // '\'

    TokenHelperLine("%", TokenTypePercentSign), // %
    TokenHelperLine("+", TokenTypePlus), // +
    TokenHelperLine("-", TokenTypeMinus), // -
    TokenHelperLine("*", TokenTypeStar), // *
    TokenHelperLine("/", TokenTypeSlash), // /
    TokenHelperLine(",", TokenTypeComma), // ,
    TokenHelperLine(".", TokenTypeDot), // .
    TokenHelperLine("f", TokenTypeF), // 'f'
    TokenHelperLine("!", TokenTypeExclamationMark), // !
    TokenHelperLine("@", TokenTypeAtSign), // @
    TokenHelperLine("&", TokenTypeBitwiseAnd), // &
    TokenHelperLine("|", TokenTypeBitwiseOr), // &
    TokenHelperLine("#", TokenTypeHashSign), // #
    TokenHelperLine("?", TokenTypeQuestionMark), // ?

    TokenHelperLine(":", TokenTypeColon), // :
    TokenHelperLine(";", TokenTypeSemiColon), // ;

    TokenHelperLine("(", TokenTypeParenthesesOpen), // ()
    TokenHelperLine(")", TokenTypeParenthesesClose),
    TokenHelperLine("[", TokenTypeSquareBracketOpen), // []
    TokenHelperLine("]", TokenTypeSquareBracketClose),
    TokenHelperLine("{", TokenTypeCurlyBracketOpen), // {}
    TokenHelperLine("}", TokenTypeCurlyBracketClose),
    TokenHelperLine("<", TokenTypeAngleBracketOpen), // <>
    TokenHelperLine(">", TokenTypeAngleBracketClose),

    TokenHelperLine("const", TokenTypeConst),
    TokenHelperLine("while", TokenTypeWhile),
    TokenHelperLine("fn", TokenTypeFunc),
    TokenHelperLine("let", TokenTypeLet),
    TokenHelperLine("var", TokenTypeVar),
};
static const s32 TokenHelperCount
    = (s32)(sizeof(tokenHelpers) / sizeof(TokenHelper));


#undef TokenHelperLine

typedef struct TokenIndex
{
    const CarpBuffer tokenIndexBuffer;
    s32 tokenIndexIndex;
} TokenIndex;

static bool sSkipWhiteSpace(TokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, false);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize, false);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, false);

    while(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        switch(tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex])
        {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            default:
                return true;
        }
        ++tokenIndex->tokenIndexIndex;
    }
    return false;
}


static s32 sTryParseIdentifier(const TokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, -1);

    char c = tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex];
    if(!(
        (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || c == '_'))
    {
        return 0;
    }
    s32 len = 1;

    while(tokenIndex->tokenIndexIndex + len < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        char c = tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex + len];
        if(!(
            (c >= 'a' && c <= 'z')
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || c == '_'))
        {
            return len;
        }
        ++len;
    }
    return -1;
}

static s32 sTryParseNumber(const TokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < tokenIndex->tokenIndexBuffer.carpBufferSize, -1);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, -1);

    s32 len = 0;

    while(tokenIndex->tokenIndexIndex + len < tokenIndex->tokenIndexBuffer.carpBufferSize)
    {
        char c = tokenIndex->tokenIndexBuffer.carpBufferData[tokenIndex->tokenIndexIndex + len];
        if(c < '0' || c > '9')
        {
            return len;
        }
        ++len;
    }
    return -1;
}

static bool sPushToken(const Token* token, CarpBuffer *outTokenBuffer)
{
    return carp_buffer_pushBuffer(outTokenBuffer, (const u8*)token, sizeof(Token));
}

static bool sParseTokens(TokenIndex* tokenIndex, CarpBuffer *outTokenBuffer)
{
    CARP_ASSERT_RETURN(tokenIndex, false);
    CARP_ASSERT_RETURN(outTokenBuffer, false);

    const CarpBuffer* input = &(tokenIndex->tokenIndexBuffer);
    CARP_ASSERT_RETURN(input->carpBufferSize > 0, false);
    CARP_ASSERT_RETURN(input->carpBufferData, false);
    CARP_ASSERT_RETURN(input->carpBufferData[input->carpBufferSize - 1] == '\0', false);

    s32 capacity = (1 << 18) * (sizeof(Token) + 4);
    CARP_ASSERT_RETURN(carp_buffer_create(capacity, 256, outTokenBuffer), NULL);

    while(tokenIndex->tokenIndexIndex < input->carpBufferSize)
    {
        Token pushToken = { 0 };

        CARP_ASSERT_RETURN(sSkipWhiteSpace(tokenIndex), false);
        CARP_ASSERT_RETURN(tokenIndex->tokenIndexIndex < input->carpBufferSize, false);

        pushToken.tokenStart = tokenIndex->tokenIndexIndex;
        const uint8_t* pos = input->carpBufferData + tokenIndex->tokenIndexIndex;

        s32 idLen = sTryParseIdentifier(tokenIndex);
        CARP_ASSERT_RETURN(idLen >= 0, false);

        if(idLen > 0)
        {
            pushToken.tokenType = TokenTypeIdentifier;
            pushToken.tokenLen = idLen;
        }

        for(s32 helperIndex = 0; helperIndex < TokenHelperCount; ++helperIndex)
        {
            const TokenHelper* helper = tokenHelpers + helperIndex;
            if(idLen <=  helper->tokenHelperLen
                && memcmp(helper->tokenHelperStr, pos, helper->tokenHelperLen) == 0)
            {
                pushToken.tokenType = helper->tokenHelperType;
                pushToken.tokenLen = helper->tokenHelperLen;

                if(helper->tokenHelperType == TokenTypeEndOfTokens)
                {
                    return true;
                }
                break;
            }
        }
        if(pushToken.tokenType == TokenTypeUnknownToken)
        {
            s32 numLen = sTryParseNumber(tokenIndex);
            CARP_ASSERT_RETURN(numLen > 0, false);

            pushToken.tokenLen = numLen;
            pushToken.tokenType = TokenTypeNumber;
        }
        CARP_ASSERT_RETURN(pushToken.tokenType != TokenTypeUnknownToken, false);

        tokenIndex->tokenIndexIndex += pushToken.tokenLen;
        CARP_ASSERT_RETURN(sPushToken(&pushToken, outTokenBuffer), false);
    }

    return false;
}
void sPrintErrorAt(const TokenIndex* tokenIndex)
{
    CARP_ASSERT_RETURN(tokenIndex, ;);
    CARP_ASSERT_RETURN(tokenIndex->tokenIndexBuffer.carpBufferData, ;);

    s32 lineNumber = 0;
    s32 charNumber = 0;

    s32 index = 0;
    while(index < tokenIndex->tokenIndexIndex)
    {
        char c = (char)tokenIndex->tokenIndexBuffer.carpBufferData[index];
        if(c == '\n')
        {
            lineNumber++;
            charNumber = 0;
        }
        ++index;
    }

    CARP_LOGERROR("Error at: %i:%i \"%.5s\"\n",
        lineNumber,
        charNumber,
        tokenIndex->tokenIndexBuffer.carpBufferData + index);

}
void sPrintTokens(const CarpBuffer* input, const CarpBuffer* tokenBuffer)
{
    const Token* token = (const Token*)tokenBuffer->carpBufferData;
    const Token* endToken = (const Token*)
        (tokenBuffer->carpBufferData + tokenBuffer->carpBufferSize);
    s32 tokenCount = 0;
    while(token < endToken)
    {
        const char* name = "";
        switch(token->tokenType)
        {
            case TokenTypeUnknownToken: name = "Unknown"; break;
            case TokenTypeEndOfTokens: name = "End Of Tokens"; break;
            case TokenTypePercentSign: name = "%"; break;
            case TokenTypePlus: name = "+"; break;
            case TokenTypeMinus: name = "-"; break;
            case TokenTypeStar: name = "*"; break;
            case TokenTypeSlash: name = "/"; break;
            case TokenTypeBackSlash: name = "\\"; break;
            case TokenTypeComma: name = ","; break;
            case TokenTypeDot: name = "."; break;
            case TokenTypeF: name = "f"; break;
            case TokenTypeSetEqual: name = ":="; break;
            case TokenTypeNotEqual: name = "!="; break;
            case TokenTypeDoubleEqual: name = "=="; break;
            case TokenTypeDoubleQuote: name = "\""; break;
            case TokenTypeExclamationMark: name = "!"; break;
            case TokenTypeAtSign: name = "@"; break;
            case TokenTypeBitwiseAnd: name = "&"; break;
            case TokenTypeBitwiseOr: name = "|"; break;
            case TokenTypeLogicalAnd: name = "&&"; break;
            case TokenTypeLogicalOr: name = "||"; break;
            case TokenTypeHashSign: name = "#"; break;
            case TokenTypeQuestionMark: name = "?"; break;
            case TokenTypeColon: name = ":"; break;
            case TokenTypeSemiColon: name = ";"; break;
            case TokenTypeParenthesesOpen: name = "("; break;
            case TokenTypeParenthesesClose: name = ")"; break;
            case TokenTypeSquareBracketOpen: name = "["; break;
            case TokenTypeSquareBracketClose: name = "]"; break;
            case TokenTypeCurlyBracketOpen: name = "{"; break;
            case TokenTypeCurlyBracketClose: name = "}"; break;
            case TokenTypeAngleBracketOpen: name = "<"; break;
            case TokenTypeAngleBracketClose: name = ">"; break;

            // Number type
            case TokenTypeNumber: name = "Number"; break;

            // Keywords
            case TokenTypeConst: name = "Const"; break;
            case TokenTypeWhile: name = "While"; break;
            case TokenTypeFunc: name = "Func"; break;
            case TokenTypeLet: name = "Let"; break;
            case TokenTypeVar: name = "Var"; break;

            case TokenTypeIdentifier: name = "Identifier"; break;
            case TokenTypeCount: name = "Count"; break;
        }
        if((tokenCount % 8) == 0)
        {
            CARP_LOG("\n");
        }
        if(token->tokenType == TokenTypeIdentifier
            || token->tokenType == TokenTypeNumber)
        {
            CARP_LOG("'%s[%-.*s]', ",
                name,
                token->tokenLen,
                (const char*)(input->carpBufferData + token->tokenStart));

        }
        else
        {
            CARP_LOG("'%s', ", name);
        }
        ++token;
        ++tokenCount;
    }
    CARP_LOG("\n");
}



CARP_FN bool carp_lang_compileToSpecialBuffer(
    const char* input,
    s32 inputLen,
    CarpSpecialBuffer* outBuffer)
{
    CARP_ASSERT_RETURN(input, false);
    CARP_ASSERT_RETURN(outBuffer, false);
    CARP_ASSERT_RETURN(inputLen > 0, false);

    CarpBuffer tokenOutBuffer = {0};
    const CarpBuffer inputBuffer = {
            .carpBufferData = (u8*)input,
            .carpBufferCapacity = inputLen,
            .carpBufferSize = inputLen,
            .carpBufferAlignment = 1,
    };

    TokenIndex tokenIndex = {.tokenIndexBuffer = inputBuffer };
    if(!sParseTokens(&tokenIndex, &tokenOutBuffer))
    {
        sPrintErrorAt(&tokenIndex);
        CARP_LOGERROR("Failed to parse tokens\n");
        return false;
    }
    sPrintTokens(&tokenIndex.tokenIndexBuffer, &tokenOutBuffer);

    carp_buffer_free(&tokenOutBuffer);
    return true;
}
