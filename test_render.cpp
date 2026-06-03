#include "TuiLogin.h"
#include <iostream>
#include <unistd.h>

int main() {
    setlocale(LC_ALL, "");
    notcurses_options opts{};
    opts.flags = NCOPTION_SUPPRESS_BANNERS | NCOPTION_INHIBIT_SETLOCALE;
    struct notcurses* nc = notcurses_init(&opts, nullptr);
    if (!nc) return 1;

    // Simulate TuiLoginScreen layout logic explicitly to see if it fails
    unsigned int termRows, termCols;
    notcurses_term_dim_yx(nc, &termRows, &termCols);

    struct ncplane* stdPlane = notcurses_stdplane(nc);
    ncplane_set_bg_rgb8(stdPlane, 0x08, 0x0E, 0x17);
    ncplane_set_fg_rgb8(stdPlane, 0x12, 0x24, 0x36);
    ncplane_erase(stdPlane);

    ncplane_options nopts{};
    nopts.y = (termRows - 18) / 2;
    if (nopts.y < 0) nopts.y = 0;
    nopts.x = (termCols - 52) / 2;
    if (nopts.x < 0) nopts.x = 0;
    nopts.rows = 18;
    nopts.cols = 52;
    nopts.flags = 0;

    struct ncplane* boxPlane = ncplane_create(stdPlane, &nopts);
    if (!boxPlane) {
        notcurses_stop(nc);
        std::cerr << "Failed to create boxPlane\n";
        return 1;
    }

    ncplane_set_bg_rgb8(boxPlane, 0x0D, 0x1B, 0x2A);
    ncplane_erase(boxPlane);

    ncplane_set_fg_rgb8(boxPlane, 0x4F, 0xC3, 0xF7);
    ncplane_putstr_yx(boxPlane, 1, 1, "HELLO MODAL WINDOW!");

    int ret = notcurses_render(nc);
    if (ret != 0) {
        notcurses_stop(nc);
        std::cerr << "notcurses_render failed with " << ret << "\n";
        return 1;
    }

    sleep(2);
    notcurses_stop(nc);
    std::cout << "Success\n";
    return 0;
}
