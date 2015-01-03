/*
 * Copyright (C) 1999-2011  Lorenzo Bettini <http://www.lorenzobettini.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "docgenerator.h"

using std::endl;

namespace srchilite {

void
DocGenerator::generate_start_doc(std::ostream *sout)
{
    bool docTitle = (title.size () > 0);
    bool docInputLang = (input_lang.size () > 0);
    bool inputFileName = (input_file_name.size () > 0);

    *sout <<
        docTemplate.output_begin(
            ( docTitle ? title :
                  ( inputFileName ? input_file_name : "source file" ) ),
            css_url,
            "",
            doc_header,
            doc_footer,
            doc_background,
	    docInputLang ? input_lang: "unknown"
        );
}

void
DocGenerator::generate_end_doc(std::ostream *sout)
{
    *sout <<
        docTemplate.output_end(
            "",
            css_url,
            "",
            doc_header,
            doc_footer,
            doc_background,
	    ""
        );
}

}
