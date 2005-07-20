/***************************************************************************
 *  The Kernel-Machine Library                                             *
 *  Copyright (C) 2004, 2005 by Rutger W. ter Borg                         *
 *                                                                         *
 *  This program is free software; you can redistribute it and/or          *
 *  modify it under the terms of the GNU General Public License            *
 *  as published by the Free Software Foundation; either version 2         *
 *  of the License, or (at your option) any later version.                 *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307  *
 ***************************************************************************/

#ifndef IO_HPP
#define IO_HPP

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/range/size.hpp>
#include <boost/range/value_type.hpp>
#include <iostream>
#include <fstream>
#include <string>

namespace kml {


/*!
Read a data file also used in Joachims' SVM_Light format
For more details, see http://svmlight.joachims.org/
 
 
\param I a range over input examples
\param O a range over output examples
 
The routine performs two passes over the data file (mainly to 
preserve the maximum amount of memory needed): 
 
-1 Acquire statistics of the file
 
   - How many data samples are we dealing with?
   - What is the maximum number of nonzero attributes found in a sample?
   - What is the maximum index of attribute found in a sample?
 
-2 Read and parse all values in the file

Everything after a '#' symbol is discarded.

*/



template<typename I, typename O>
void read_svm_light( char* file_name, I &inputs, O &outputs ) {

    typedef typename boost::range_value<I>::type input_type;
    typedef typename boost::range_value<O>::type output_type;

    // TODO
    typedef double scalar_type;

    std::size_t number_of_samples = 0;
    std::size_t maximum_nonzeros = 0;
    std::size_t maximum_index = 0;
    std::string line_buffer;

    // open the file
    std::ifstream data( file_name, std::ios::in );

    // these are the separators the input file will be split on:
    // space, tab, #, and :
    boost::char_separator<char> separator(" \t#:");

    // PASS 1: determine some file statistics
    while( !data.eof() ) {
        std::getline( data, line_buffer );
        if ( line_buffer[0] != static_cast<char>('#') ) {
            boost::tokenizer<boost::char_separator<char> > tokens( line_buffer, separator );
            typedef boost::tokenizer<boost::char_separator<char> >::iterator iter;
            iter i=tokens.begin();
            if ( i != tokens.end() ) {
                // skip the output value
                ++i;
                std::size_t nonzeros = 0;
                while( (i != tokens.end()) && ((*i)[0] != static_cast<char>('#')) ) {
                    unsigned int attribute_nr = boost::lexical_cast< unsigned int >( *i++ );
                    if (attribute_nr > maximum_index)
                        maximum_index = attribute_nr;
                    if ( i == tokens.end() ) {
                        std::cerr << "Error in input file!" << std::endl;
                        break;
                    }
                    // skip the input attribute value
                    ++nonzeros;
                    ++i;
                }
                ++number_of_samples;
                if (nonzeros > maximum_nonzeros)
                    maximum_nonzeros = nonzeros;
            }
        }
    }

    //     std::cout << "Number of samples: " << number_of_samples << std::endl;
    //     std::cout << "Maximum nonzeros:  " << maximum_nonzeros << std::endl;
    //     std::cout << "Maximum index:     " << maximum_index << std::endl;

    // resize the inputs and outputs containers accordingly
    inputs.resize( number_of_samples );
    outputs.resize( number_of_samples );

    std::size_t sample_index = 0;

    // PASS 2: go back to the beginning of the file and read all data (for real, now)
    data.clear();
    data.seekg(std::ios::beg);
    while( !data.eof() ) {

        // read a line
        std::getline( data, line_buffer );

        // ignore entire line if first character is a #
        if ( line_buffer[0] != static_cast<char>('#') ) {

            // instantiate a tokenizer, of which the tokens will be iterated through
            // From doc: parsing is done on demand (!!) as the tokens are accessed via the iterator provided by begin.
            // This implies that the size is unknown
            boost::tokenizer<boost::char_separator<char> > tokens( line_buffer, separator );
            typedef boost::tokenizer<boost::char_separator<char> >::iterator iter;

            // go to the first token. This should be the output.
            iter i=tokens.begin();

            // only if more than one token
            if ( i != tokens.end() ) {

                // translate the output type from string to the requested output type
                // add this output to the outputs (which has to comply to a back inserter container)
                outputs[ sample_index ] = boost::lexical_cast< output_type >( *i );

                // proceed to the first input attribute
                ++i;

                // TODO Sparse allocation!
                // Create an input_type object of the right size (we assume its a DENSE vector a.t.m., sparse to be done!!)

                // DENSE
                inputs[ sample_index ] = input_type( maximum_index );

                // SPARSE
                // inputs[ sample_index ] = input_type( maximum_index, maximum_nonzeros );

                // Right part of this condition is not evaluated if at end, as per
                // language standards. So (*i)[0] exists if it reaches that part.
                while( (i != tokens.end()) && ((*i)[0] != static_cast<char>('#')) ) {
                    // Figure out the attribute number. This is 1-based, so subtract 1
                    unsigned int attribute_nr = boost::lexical_cast< unsigned int >( *i++ ) - 1;
                    if ( i == tokens.end() ) {
                        std::cerr << "Error in input file!" << std::endl;
                        break;
                    }
                    // read the input into the input container
                    inputs[sample_index][attribute_nr] = boost::lexical_cast< scalar_type >( *i++ );
                }
            }
            ++sample_index;
        }
    }
    data.close();
}








}


#endif

