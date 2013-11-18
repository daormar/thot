//
// File: preprocess.h
//
// The header for the preprocessing
//

#ifndef PREPROCESS_H
#define PREPROCESS_H

#include <string>
#include <vector>

#define myisupper(c) ((c>=65) && (c<=90)) 
#define myislower(c) ((c>=97) && (c<=122)) 

/**\file preprocess.h

\brief The preprocessing of the input.
*/

/**\brief The class that stores the info about the labels
   in the input.

   For each of bullet, key, number and other there is an array
   where the corresponding values are stored. They are later
   retrieved in FIFO fashion. They can be retrieved as many
   times as necessary by reseting the counters. In case there
   are more retrievals than insertions, the last value is
   returned repeatedly. The strings are copied in the structure.
*/

class Tables
  {
  public:
    /** \brief Clear all the information. */
    void clear()
      {
	bullets.clear();
	keys.clear();
	numbers.clear();
	others.clear();
	resetCounters();
      }
    /** \brief Reset the counters for a new retrieval cycle. */
    void resetCounters()
      {
	posbullet= poskey= posnumber= posother= 0;
      }
    /** \brief Adds a new bullet. */
    void addBullet(const char *s)
      {
	bullets.push_back(s);
      }
    /** \brief Adds a new key. */
    void addKey(const char *s)
      {
	keys.push_back(s);
      }
    /** \brief Adds a new number. */
    void addNumber(const char *s)
      {
	numbers.push_back(s);
      }
    /** \brief Adds a new "other". */
    void addOther(const char *s)
      {
	others.push_back(s);
      }
    /** \brief Retrieves a bullet. */
    const char *retrieveBullet()
      {
	return retrieve(bullets, posbullet);
      }
    /** \brief Retrieves a key. */
    const char *retrieveKey()
      {
	return retrieve(keys, poskey);
      }
    /** \brief Retrieves a number. */
    const char *retrieveNumber()
      {
	return retrieve(numbers, posnumber);
      }
    /** \brief Retrieves an "other". */
    const char *retrieveOther()
      {
	return retrieve(others, posother);
      }

  private:
    std::vector<std::string> bullets, /**<The bullets. */
      keys, /**<The keys. */
      numbers, /**<The numbers. */
      others; /**<The others. */
    size_t posbullet, /**<The position of the next unread bullet. */
      poskey, /**<The position of the next unread key. */
      posnumber, /**<The position of the next unread number. */
      posother; /**<The position of the next unread other. */

    /**\brief Retrieves a string from a given position of a vector
       and increases that position.

    If the position is out of the bounds, the last element of the
    vector is returned, if exists, if the vector is empty, the empty
    string is returned. */
    const char *retrieve(const std::vector<std::string> &v, /**<The vector. */
			 size_t &pos /**<The position. */)
      {
	if ( pos>= v.size() )
	  if ( v.size() )
	    return v.back().c_str();
	  else
	    return "";
	else
	  return v[pos++].c_str();
      }

  };

/**\brief The tables storing the labels. */
extern Tables XRCEpprocTables;
extern Tables XRCEcategTables;
extern Tables EUpprocTables;

/**\brief Preprocesses a line. The returned string is static and changes after
each call.
*/
const char *XRCEpprocLine(const char *line, /**<The input line. */
		      const bool lowcase= true, /**<If true, the sentence is converted to lowercase. */
		      const bool updateTables= true /**<If true, the tables are updated
						       during the preprocessing. If false,
						       no table is modified. */
		      );



/* pproc should be equivalent to tokenization + categorization */

/**\brief Just tokenizes a line. The returned string is static and changes after
each call.
*/
const char *XRCEtokLine(const char *line, /**<The input line. */
		      const bool lowcase= true, /**<If true, the sentence is converted to lowercase. */
		      const bool updateTables= true /**<If true, the tables are updated
						       during the preprocessing. If false,
						       no table is modified. */
		      );

/**\brief Just categorizes a line. The returned string is static and changes after
each call.
*/
const char *XRCEcategLine(const char *line, /**<The input line. */
		      const bool updateTables= true /**<If true, the tables are updated
						       during the preprocessing. If false,
						       no table is modified. */
		      );



/**\brief Initializes the nouns map with the given
   file. */
void EUpostprocInitializeCapitalization(std::istream &is);

/**\brief Initializes the nouns map with the given
   file. */
void EUpostprocInitializeExceptions(std::istream &is);


/**\brief Preprocesses a line. The returned string is static and changes after
each call.
*/
const char *EUpprocLine(const char *line, /**<The input line. */
                      const bool lowcase= true, /**<If true, the sentence is converted to lowercase. */
                      const bool updateTables= true /**<If true, the tables are updated
                                                       during the preprocessing. If false,
                                                       no table is modified. */
                      );



const char *whiteCollapse(const char *line);


#endif
