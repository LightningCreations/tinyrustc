#ifndef _TRUSTC_LEX_HXX_2022_11_09_14_01_52
#define _TRUSTC_LEX_HXX_2022_11_09_14_01_52

#include <variant.hxx>
#include <string>
#include <hash.hxx>
#include <optional>

namespace trustc::lex{
    

    enum class identifier_kind{
        Normal,
        Raw,
        Keyword
    };


    struct identifier{
        identifier_kind kind;
        std::u32string identifier;
    };

    enum class sigil{
        Plus,
        Minus,
        Star,
        Slash,
        Percept,
        Caret,
        Not,
        And,
        Or,
        AndAnd,
        OrOr,
        Shl,
        Shr,
        PlusEq,
        MinusEq,
        StarEq,
        SlashEq,
        PercentEq,
        CaretEq,
        AndEq,
        OrEq,
        ShlEq,
        ShrEq,
        Eq,
        EqEq,
        Ne,
        Gt,
        Lt,
        Ge,
        Le,
        At,
        Underscore,
        Dot,
        DotDot,
        DotDotDot,
        DotDotEq,
        Coma,
        Semi,
        Colon,
        PathSep,
        RArrow,
        FatArrow,
        Pound,
        Dollar,
        Question
    };

    enum class string_literal_kind : std::uint32_t{
        Utf8,
        Byte,


        Raw = 0x80000000
    };

    constexpr string_literal_kind operator |(string_literal_kind a, string_literal_kind b) noexcept{
        return string_literal_kind{static_cast<std::uint32_t>(a)&static_cast<std::int32_t>(b)};
    }

    constexpr string_literal_kind operator &(string_literal_kind a, string_literal_kind b) noexcept{
        return string_literal_kind{static_cast<std::uint32_t>(a)&static_cast<std::int32_t>(b)};
    }

    constexpr string_literal_kind operator~(string_literal_kind a) noexcept{
        return string_literal_kind{~static_cast<std::uint32_t>(a)};
    }


    struct string_literal{
        string_literal_kind kind;
        std::u8string lit;
    };

    struct char_literal{
        string_literal_kind kind;
        std::u8string lit;
    };

    struct lifetime{
        std::u32string life;
    };

    enum class group_kind{
        Paren,
        Brace,
        Bracket,
        None,
    };

    enum class token_type{
        Identifier,
        StringLiteral,
        CharLiteral,
        IntLiteral,
        FloatLiteral,
        Lifetime,
        Sigil,
        Group,
    };

    struct token;

    struct group{
        group_kind kind;
        std::vector<token> toks;
    };

    struct token{
        trustc::variant<token_type, identifier, string_literal, char_literal, uint128, double, lifetime, sigil, group> token; 
    };


    std::vector<token> lex(std::basic_istream<char32_t>& is, std::optional<char32_t> stopc=std::nullopt);
    
}

#endif /* _TRUSTC_LEX_HXX_2022_11_09_14_01_52 */