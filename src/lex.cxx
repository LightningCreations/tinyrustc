
#include <utility.hxx>

#include <lex.hxx>

#include <unicode.hxx>

#include <cuchar>

using namespace trustc::lex;

std::vector<token> trustc::lex::lex(std::basic_istream<char32_t>& is, std::optional<char32_t> stopc){
    std::vector<token> tok{};
    do{
        auto i = is.peek();
        if(i==-1)
            break;
        if(i==stopc)
            break;

        if(trustc::is_xid_start(i)||i==U'_'){
            std::u32string id_word{};
            do{
                is.get();
                id_word.push_back(i);
                i = is.peek();
            }while(trustc::is_xid_continue(i));
            

            if(i==U'#'){
                // todo
            }else{
                auto id_kind = trustc::string_switch(id_word)
                    .mutlimatch([]{return identifier_kind::Keyword;}, U"as",U"break",U"const",U"continue",U"crate",U"else",U"enum", U"extern", U"false", U"fn", U"for",
                         U"if", U"impl", U"in", U"let", U"loop", U"match", U"mod", U"mut", U"pub", U"ref", U"return", U"self", U"Self",U"static",U"struct", U"super", U"trait",
                         U"true", U"type", U"unsafe", U"use", U"where", U"while",U"async",U"await",U"dyn",U"abstract",U"become",U"box",U"do",U"final",U"macro",
                         U"override",U"priv",U"typeof",U"yield",U"try")
                    .or_else([](auto _dontcare){return identifier_kind::Normal;});

            }


        }
    }while(is);

    return tok;
}
