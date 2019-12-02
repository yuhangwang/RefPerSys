/****************************************************************
 * file store_rps.cc
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *
 *      It has the code for the persistent store, in HJSON format.
 *      See also http://hjson.org/ & https://github.com/hjson/hjson-cpp
 *
 * Author(s):
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *      Nimesh Neema <nimeshneema@gmail.com>
 *
 *      © Copyright 2019 The Reflective Persistent System Team
 *      team@refpersys.org & http://refpersys.org/
 *
 * License:
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include "refpersys.hh"

extern "C" const char rps_store_gitid[];
const char rps_store_gitid[]= RPS_GITID;

extern "C" const char rps_store_date[];
const char rps_store_date[]= __DATE__;

//////////////////////////////////////////////////////////////// dump
void rps_dump_into (const std::string dirpath)
{
  RPS_FATAL("unimplemented rps_dump_into '%s'", dirpath.c_str());
#warning rps_dump_into unimplemented
} // end of rps_dump_into

//////////////////////////////////////////////////////////////// load
void rps_load_from (const std::string& dirpath)
{
  RPS_WARN("unimplemented rps_load_from '%s'", dirpath.c_str());
#warning rps_load_from unimplemented
} // end of rps_load_from

//////////////////////////////////////////////////////////// end of file store_rps.cc

