

/****************************************************************************** 
 * 
 *  file:  ValuesConstraint.h
 * 
 *  Copyright (c) 2005, Michael E. Smoot
 *  All rights reverved.
 * 
 *  See the file COPYING in the top directory of this distribution for
 *  more information.
 *  
 *  THE SOFTWARE IS PROVIDED _AS IS_, WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 *  DEALINGS IN THE SOFTWARE.  
 *  
 *****************************************************************************/

#ifndef TCLAP_VALUESCONSTRAINT_H
#define TCLAP_VALUESCONSTRAINT_H

#include <cassert>
#include <string>
#include <tclap/Constraint.hpp>
#include <vector>



#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define HAVE_SSTREAM
#endif

#if defined(HAVE_SSTREAM)
#include <sstream>
using streamtype = std::ostringstream;
#elif defined(HAVE_STRSTREAM)
#include <strstream>
typedef std::ostrstream streamtype;
#else
#error "Need a stringstream (sstream or strstream) to compile!"
#endif

namespace TCLAP {

/**
 * A Constraint that constrains the Arg to only those values specified
 * in the constraint.
 */
template <class T>
class ValuesConstraint : public Constraint<T> {

  public:
    /**
		 * Constructor. 
		 * \param _allowed - vector of allowed values. 
		 */
    explicit ValuesConstraint(std::vector<T> _allowed);

    /**
		 * Virtual destructor.
		 */
    ~ValuesConstraint() override = default;

    ValuesConstraint(const ValuesConstraint&) = default;
    ValuesConstraint(ValuesConstraint&&) = default;
    ValuesConstraint& operator=(const ValuesConstraint&) = default;
    ValuesConstraint& operator=(ValuesConstraint&&) = default;

    /**
		 * Returns a description of the Constraint. 
		 */
    const std::string& description() const override;

    /**
		 * Returns the short ID for the Constraint.
		 */
    const std::string& shortID() const override;

    /**
		 * The method used to verify that the value parsed from the command
		 * line meets the constraint.
		 * \param val - The value that will be checked. 
		 */
    bool check(const T& val) const override;

  protected:
    /**
		 * The list of valid values. 
		 */
    std::vector<T> m_allowed;

    /**
		 * The string used to describe the allowed values of this constraint.
		 */
    std::string m_typeDesc;
};

template <class T>
ValuesConstraint<T>::ValuesConstraint(std::vector<T> _allowed)
    : m_allowed(std::move(_allowed))
    , m_typeDesc([&]() {
        assert(int(m_allowed.size()) > 0);

        streamtype os;
        os << m_allowed[0];
        for (int i = 1; i < m_allowed.size(); ++i) {
            os << '|' << m_allowed[i];
        }
        return os.str();
    }()) {
}

template <class T>
bool ValuesConstraint<T>::check(const T& val) const {
    return std::find(m_allowed.begin(), m_allowed.end(), val) != m_allowed.end();
}

template <class T>
const std::string& ValuesConstraint<T>::shortID() const {
    return m_typeDesc;
}

template <class T>
const std::string& ValuesConstraint<T>::description() const {
    return m_typeDesc;
}

} //namespace TCLAP
#endif
