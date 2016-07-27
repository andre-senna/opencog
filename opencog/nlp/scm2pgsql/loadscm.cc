/*
 * loadscm.cc
 *
 * Copyright (C) 2016 OpenCog Foundation
 *
 * Author: Andre Senna <https://github.com/andre-senna>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "SCMLoader.h"
#include "TypeFrameIndex.h"
#include "TypeFrameIndexBuilder.h"
#include <opencog/guile/SchemeEval.h>

using namespace opencog;

bool loadFile(char *fileName)
{
    TypeFrameIndex index;
    TypeFrameIndexBuilder builder(&index);
    AtomSpace atomSpace;
    SchemeEval::init_scheme();
    SchemeEval::set_scheme_as(&atomSpace);
    SchemeEval *schemeEval = new SchemeEval(&atomSpace);
    schemeEval->eval("(use-modules (opencog nlp relex2logic))");
    bool returnValue = SCMLoader::load(fileName, atomSpace, &builder);
    //bool returnValue = SCMLoader::load(fileName, atomSpace);
    index.printForDebug();
    return returnValue;
}

int main(int argc, char *argv[]) {

    int exitValue = 0;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <SCM file>\n", argv[0]);
        exitValue = 1;
    } else {
        if (! loadFile(argv[1])) {
            exitValue = 1;
        }
    }

    return exitValue;
}
