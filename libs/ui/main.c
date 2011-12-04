//
//
//    Copyright 2011 TheSeven
//
//
//    This file is part of emCORE.
//
//    emCORE is free software: you can redistribute it and/or
//    modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the
//    License, or (at your option) any later version.
//
//    emCORE is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//    See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with emCORE.  If not, see <http://www.gnu.org/licenses/>.
//
//


#include "emcorelib.h"
#include "export/libui.h"


static struct libui_api apitable =
{
    .fill = fill,
    .blit = blit,
    .blendcolor = blendcolor,
    .mattecolor = mattecolor,
    .blend = blend,
    .blenda = blenda,
    .chooser_run = chooser_run,
    .chooser_action_handler_wheel = &chooser_action_handler_wheel,
    .chooser_renderer_list = &chooser_renderer_list,
    .chooser_renderer_iconflow = &chooser_renderer_iconflow,
    .chooser_renderer_list_render_attached_text = &chooser_renderer_list_render_attached_text,
    .chooser_renderer_list_show_arrow_right = &chooser_renderer_list_show_arrow_right,
    .chooser_renderer_list_show_arrow_left = &chooser_renderer_list_show_arrow_left,
    .settingchooser_run = &settingchooser_run
};

EMCORE_LIB_HEADER(LIBUI_IDENTIFIER, LIBUI_API_VERSION, LIBUI_MIN_API_VERSION, NULL, NULL, apitable)
