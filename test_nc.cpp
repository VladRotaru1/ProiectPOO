#include <notcurses/notcurses.h>
#include <unistd.h>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "");
    notcurses_options opts{};
    opts.flags = NCOPTION_SUPPRESS_BANNERS | NCOPTION_INHIBIT_SETLOCALE;
    struct notcurses* nc = notcurses_init(&opts, nullptr);
    if (!nc) return 1;

    struct ncplane* stdn = notcurses_stdplane(nc);
    ncplane_set_fg_rgb8(stdn, 255, 0, 0);
    ncplane_putstr_yx(stdn, 2, 2, "HELLO NOTCURSES WORLD!!!");

    notcurses_render(nc);
    sleep(3);
    notcurses_stop(nc);
    return 0;
}
