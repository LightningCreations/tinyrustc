
#include <unicodelib.h>
#include <unicode.hxx>

bool trustc::is_xid_start(char32_t c){
    return unicode::is_xid_start(c);
}

bool trustc::is_xid_continue(char32_t c){
    return unicode::is_xid_continue(c);
}
