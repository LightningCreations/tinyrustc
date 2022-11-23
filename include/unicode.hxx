#ifndef _TRUSTC_UNICODE_HXX_2022_11_09_16_05_16

namespace trustc{
    bool is_xid_start(char32_t c);
    bool is_xid_continue(char32_t c);
}

#endif