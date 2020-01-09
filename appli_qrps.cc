/****************************************************************
 * file appli_qrps.cc
 *
 * Description:
 *      This file is part of the Reflective Persistent System.
 *
 *      It has the Qt5 code related to the Qt5 application
 *
 * Author(s):
 *      Basile Starynkevitch <basile@starynkevitch.net>
 *      Abhishek Chakravarti <abhishek@taranjali.org>
 *      Nimesh Neema <nimeshneema@gmail.com>
 *
 *      © Copyright 2019 - 2020 The Reflective Persistent System Team
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
#include "qthead_qrps.hh"


extern "C" const char rps_appli_gitid[];
const char rps_appli_gitid[]= RPS_GITID;

extern "C" const char rps_appli_date[];
const char rps_appli_date[]= __DATE__;

// these functions are here, because other files might not include <QString>
const Rps_String*
Rps_String::make(const QString&qs)
{
  return Rps_String::make(qs.toStdString());
} // end Rps_String::make(const QString&qs)

Rps_StringValue::Rps_StringValue(const QString&qs)
  : Rps_Value(Rps_String::make(qs), Rps_ValPtrTag{})
{
} // end of Rps_StringValue::Rps_StringValue(const QString&qs)

////////////////

static char rps_bufpath_homedir[384];

const char* rps_homedir(void)
{
  static std::mutex homedirmtx;
  std::lock_guard<std::mutex> gu(homedirmtx);
  if (RPS_UNLIKELY(rps_bufpath_homedir[0] == (char)0))
    {
      const char*rpshome = getenv("REFPERSYS_HOME");
      const char*home = getenv("HOME");
      const char*path = rpshome?rpshome:home;
      if (!path)
        RPS_FATAL("no RefPerSys home ($REFPERSYS_HOME or $HOME)");
      char* rp = realpath(path, nullptr);
      if (!rp)
        RPS_FATAL("realpath failed on RefPerSys home %s - %m",
                  path);
      if (strlen(rp) >= sizeof(rps_bufpath_homedir) -1)
        RPS_FATAL("too long realpath %s on RefPerSys home %s", rp, path);
      strncpy(rps_bufpath_homedir, rp, sizeof(rps_bufpath_homedir) -1);
    }
  return rps_bufpath_homedir;
} // end rps_homedir

void
RpsQApplication::do_add_new_window(void)
{
  std::lock_guard gu(app_mutex);
  auto window = new RpsQWindow();
#warning FIXME: reuse empty slots in app_windvec
  window->setWindowTitle(QString("RefPerSys #%1").arg(app_windvec.size()));
  window->resize (640, 480); // TODO: get dimensions from $HOME/.RefPerSys
  window->show();
  app_windvec.emplace_back(window);
  app_wndcount++;
} // end of RpsQApplication::add_new_window


void
RpsQApplication::do_dump_then_exit(QString dir)
{
  rps_dump_into(dir.toStdString());
  QApplication::exit(0);
} // end RpsQApplication::do_dump_then_exit

void
RpsQWindow::do_quit()
{
  auto res = QMessageBox::question(this, "Quit RefPerSys?",
                                   "quit without dumping the persistent state?");

  if (res == QMessageBox::Yes)
    QApplication::exit(0);
} // end RpsQWindow::do_quit

RpsQApplication::RpsQApplication(int &argc, char*argv[])
  : QApplication(argc, argv),
    app_mutex(),
    app_windvec(),
    app_wndcount (0)
{
  setApplicationName("RefPerSys");
  setApplicationVersion(rps_lastgitcommit);
  app_windvec.reserve(16);
  app_windvec.push_back(nullptr); // we don't want a 0 index.
  do_add_new_window();
} // end of RpsQApplication::RpsQApplication

void
RpsQApplication::do_remove_window(int ix)
{
  std::lock_guard guapp(app_mutex);
  int wincount = app_windvec.size();
  if (ix <= 0)
    throw RPS_RUNTIME_ERROR_OUT("do_remove_window: negative index " << ix);
  if (ix >= wincount)
    throw RPS_RUNTIME_ERROR_OUT("do_remove_window: too large index " << ix
                                << " is more than " << wincount);
  app_windvec[ix].release();

} // end RpsQApplication::do_remove_window

void
RpsQApplication::do_dump_state(QString dirpath)
{
  rps_dump_into(dirpath.toStdString());
} // end of RpsQApplication::do_dump_state

void
RpsQApplication::do_dump_current_state(void)
{
  rps_dump_into(".");
} // end of RpsQApplication::do_dump_current_state


void
RpsQApplication::do_dump_current_then_exit(void)
{
  do_dump_then_exit(".");
} // end of RpsQApplication::do_dump_current_then_exit




RpsQWindow* RpsQApplication::getWindowPtr(int ix)
{
  std::lock_guard gu(app_mutex);
  if (ix < 0)
    ix += app_windvec.size();
  if (ix <= 0 || ix > (int)app_windvec.size())
    return nullptr;
  return app_windvec.at(ix).get();
}



void rps_run_application(int &argc, char **argv)
{
  bool batch = false;
  std::string dumpdirstr;
  {
    char cwdbuf[128];
    memset (cwdbuf, 0, sizeof(cwdbuf));
    getcwd(cwdbuf, sizeof(cwdbuf)-1);
    RPS_INFORM("rps_run_application: start of %s\n"
               ".. gitid %s\n"
               ".. build timestamp %s\n"
               ".. last git commit %s\n"
               ".. md5sum %s\n"
               ".. in %s\n"
               ".. on host %s pid %d\n",
               argv[0], rps_gitid,
               rps_timestamp,
               rps_lastgitcommit,
               rps_md5sum,
               cwdbuf,
               rps_hostname(), (int)getpid());
  }
  RpsQApplication app (argc, argv);
  std::string loadtopdir(rps_topdirectory);
  {
    QCommandLineParser argparser;
    argparser.setApplicationDescription("a REFlexive PERsistent SYStem");
    argparser.addHelpOption();
    argparser.addVersionOption();
    // refpersys home
    const QCommandLineOption rpshomeOption("refpersys-home",
                                           "RefPerSys homedir, default to $REFPERSYS_HOME or $HOME", "refpersys-home");
    argparser.addOption(rpshomeOption);

    // load directory
    const QCommandLineOption loadOption(QStringList() << "L" << "load",
                                        "The load directory", "load-dir");
    argparser.addOption(loadOption);
    // random oids
    const QCommandLineOption randoidOption("random-oid",
                                           "output some random oids", "nb-oids");
    argparser.addOption(randoidOption);
    // type information
    const QCommandLineOption typeOption("type-info", "Show type information.");
    argparser.addOption(typeOption);
    // batch flag
    const QCommandLineOption batchOption(QStringList() << "B" << "batch", "batch mode, without any windows");
    argparser.addOption(batchOption);
    // number of jobs, for multi threading
    const QCommandLineOption nbjobOption(QStringList() << "j" << "jobs", "number of threads", "nb-jobs");
    argparser.addOption(nbjobOption);
    // dump after load
    const QCommandLineOption dumpafterloadOption(QStringList() << "D" << "dump", "dump after load", "dump-dir");
    argparser.addOption(dumpafterloadOption);
    //
    argparser.process(app);
    ///// --refpersys-home <dir>
    if (argparser.isSet(rpshomeOption))
      {
        const QString rhomqs = argparser.value(rpshomeOption);
        std::string rhompath = rhomqs.toStdString();
        struct stat rhomstat;
        memset (&rhomstat, 0, sizeof(rhomstat));
        if (stat(rhompath.c_str(), &rhomstat))
          RPS_FATAL("failed to stat --refpersys-home %s: %m",
                    rhompath.c_str());
        if (!S_ISDIR(rhomstat.st_mode))
          RPS_FATAL("given --refpersys-home %s is not a directory",
                    rhompath.c_str());
        if ((rhomstat.st_mode & (S_IRUSR|S_IXUSR)) !=  (S_IRUSR|S_IXUSR))
          RPS_FATAL("given --refpersys-home %s is not user readable and executable",
                    rhompath.c_str());
        char*rhomrp = realpath(rhompath.c_str(), nullptr);
        if (!rhomrp)
          RPS_FATAL("realpath failed on given --refpersys-home %s - %m",
                    rhompath.c_str());
        if (strlen(rhomrp) >= sizeof(rps_bufpath_homedir) -1)
          RPS_FATAL("too long realpath %s on given --refpersys-home %s - %m",
                    rhomrp, rhompath.c_str());
        strncpy(rps_bufpath_homedir, rhomrp, sizeof(rps_bufpath_homedir) -1);
        free (rhomrp), rhomrp = nullptr;
      };
    RPS_INFORM("using %s as the RefPerSys home directory", rps_homedir());
    //// --load <dir>
    if (argparser.isSet(loadOption))
      {
        const QString loadpathqs = argparser.value(loadOption);
        loadtopdir = loadpathqs.toStdString();
      };
    /// --type-info
    if (argparser.isSet(typeOption))
      {
        rps_print_types_info ();
        batch = true;
      }
    /// --batch or -B
    if (argparser.isSet(batchOption))
      batch = true;
    /// --random-oid <nbrand>
    if (argparser.isSet(randoidOption))
      {
        int nbrand = 5;
        const QString randoidqs = argparser.value(randoidOption);
        sscanf(randoidqs.toStdString().c_str(), "%d", &nbrand);
        if (nbrand <= 0) nbrand = 2;
        else if (nbrand > 100) nbrand = 100;
        RPS_INFORM("output of %d random objids generated on %.2f\n", nbrand,
                   rps_wallclock_real_time());
        printf("*    %-20s" "\t  %-19s" "   %-12s" "\t %-10s\n",
               " objid", "hi", "lo", "hash");
        printf("========================================================"
               "===========================\n");
        for (int ix = 0; ix<nbrand; ix++)
          {
            auto rid = Rps_Id::random();
            printf("! %22s" "\t  %19lld" " %12lld" "\t %10u\n",
                   rid.to_string().c_str(),
                   (long long) rid.hi(),
                   (long long) rid.lo(),
                   (unsigned) rid.hash());
          }
        printf("--------------------------------------------------------"
               "---------------------------\n");
        fflush(nullptr);
      };
    if (argparser.isSet(nbjobOption))
      {
        int nbjobs = 3;
        const QString nbjqs = argparser.value(nbjobOption);
        if (sscanf(nbjqs.toStdString().c_str(), "%d", &nbjobs) >= 1)
          {
            if (nbjobs <= RPS_NBJOBS_MIN)
              nbjobs = RPS_NBJOBS_MIN;
            else if (nbjobs > RPS_NBJOBS_MAX)
              nbjobs = RPS_NBJOBS_MAX;
          }
        else
          RPS_WARNOUT("invalid number of jobs (-j option) " << (nbjqs.toStdString()));
        rps_nbjobs = nbjobs;
      }
    ///// --dump <dump-dir>
    if (argparser.isSet(dumpafterloadOption))
      {
        const QString dumpqs = argparser.value(dumpafterloadOption);
        dumpdirstr = dumpqs.toStdString();
        RPS_INFORMOUT("should dump into " << dumpdirstr);
      }
  }
  RPS_INFORMOUT("using " << rps_nbjobs << " jobs (or threads)");
  rps_load_from (loadtopdir);
  if (!dumpdirstr.empty())
    {
      RPS_INFORMOUT("dumping after load to " << dumpdirstr);
      rps_dump_into(dumpdirstr);
    }
  if (!batch)
    (void) app.exec ();
} // end of rps_run_application

//////////////// moc generated file
#include "_qthead_qrps.inc.hh"

//////////////////////////////////////// end of file appli_qrps.cc
