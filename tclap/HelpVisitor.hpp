
/****************************************************************************** 
 * 
 *  file:  HelpVisitor.h
 * 
 *  Copyright (c) 2003, Michael E. Smoot .
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

#ifndef TCLAP_HELP_VISITOR_H
#define TCLAP_HELP_VISITOR_H

#include "ArgException.hpp"
#include "CmdLineInterface.hpp"
#include "CmdLineOutput.hpp"
#include "Visitor.hpp"

namespace TCLAP {

/**
 * A Visitor object that calls the usage method of the given CmdLineOutput
 * object for the specified CmdLine object.
 */
class HelpVisitor: public Visitor
{
	protected:

		/**
		 * The CmdLine the output will be generated for. 
		 */
		CmdLineInterface& _cmd;

		/**
		 * The output object. 
		 */
		CmdLineOutput& _out;

	public:

		/**
		 * Constructor.
		 * \param cmd - The CmdLine the output will be generated for.
		 * \param out - The type of output. 
		 */
		HelpVisitor(CmdLineInterface& cmd, CmdLineOutput& out) : 			 
			_cmd( cmd ), 
			_out( out ) 
		{}

		HelpVisitor(const HelpVisitor&) = delete;
		HelpVisitor& operator=(const HelpVisitor&) = delete;
		HelpVisitor(HelpVisitor&&) = default;
		HelpVisitor& operator=(HelpVisitor&&) = default;
		~HelpVisitor() override = default;
		/**
		 * Calls the usage method of the CmdLineOutput for the 
		 * specified CmdLine.
		 */
		void visit() override { 
			_out.usage(_cmd); 
			throw ExitException(0); 
		}
		
};

}  // namespace TCLAP

#endif
