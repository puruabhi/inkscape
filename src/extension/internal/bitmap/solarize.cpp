/*
 * Copyright (C) 2007 Authors:
 *   Christopher Brown <audiere@gmail.com>
 *   Ted Gould <ted@gould.cx>
 *   
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include "extension/effect.h"
#include "extension/system.h"

#include "solarize.h"

namespace Inkscape {
namespace Extension {
namespace Internal {
namespace Bitmap {
	
void
Solarize::applyEffect(Magick::Image* image) {
	image->solarize(_factor);
}

void
Solarize::refreshParameters(Inkscape::Extension::Effect* module) {
	_factor = module->get_param_float("factor");
}

#include "../clear-n_.h"

void
Solarize::init(void)
{
	Inkscape::Extension::build_from_mem(
		"<inkscape-extension>\n"
			"<name>" N_("Solarize") "</name>\n"
			"<id>org.inkscape.effect.bitmap.solarize</id>\n"
			"<param name=\"factor\" gui-text=\"" N_("Factor") "\" type=\"float\" min=\"0\" max=\"100\">50</param>\n"
			"<effect>\n"
				"<object-type>all</object-type>\n"
				"<effects-menu>\n"
					"<submenu name=\"" N_("Raster") "\" />\n"
				"</effects-menu>\n"
				"<menu-tip>" N_("Solarize selected bitmap(s), like overexposing photographic film.") "</menu-tip>\n"
			"</effect>\n"
		"</inkscape-extension>\n", new Solarize());
}

}; /* namespace Bitmap */
}; /* namespace Internal */
}; /* namespace Extension */
}; /* namespace Inkscape */
