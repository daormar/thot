//
// File: postprocess.h
//
// The header for the postprocessing
//

#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <iostream>

/**\file postprocess.h

\brief The postprocessing of the input.
*/

/**\brief Initializes the capitalization map with the given
   file. */
void XRCEpostprocInitializeCapitalization(std::istream &is);



/**\brief Preprocesses a line. The returned string is static and changes after
each call.
*/
const char *XRCEpostprocLine(const char *line, /**<The input line. */
			 const bool caps= true /**<If true, the output must be capitalized. */
			 );

/**\brief Preprocesses a line. The returned string is static and changes after
each call.
*/
const char *XRCEdecategLine(const char *line);

const char *XRCEdetokLine(const char *line, /**<The input line. */
			 const bool caps= true /**<If true, the output must be capitalized. */
			 );


/**\brief Preprocesses a line. The returned string is static and changes after
each call.
*/
const char *EUpostprocLine(const char *line, /**<The input line. */
                         const bool caps= true /**<If true, the output must be capitalized. */
                         );



#endif
