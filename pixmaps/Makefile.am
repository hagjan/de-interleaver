iconsdir = $(datadir)/icons/hicolor


nobase_dist_icons_DATA = \
	16x16/apps/de-interleaver.png \
	24x24/apps/de-interleaver.png \
	32x32/apps/de-interleaver.png \
	48x48/apps/de-interleaver.png 

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(iconsdir)

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating GTK icon cache."; \
		$(gtk_update_icon_cache); \
	else \
		echo "*** Icon cache not updated. After (un)install, run this:"; \
		echo "*** $(gtk_update_icon_cache)" \
	fi

