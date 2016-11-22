// Copyright 2015, Alessandro Fabbri

/************************************************************************
* This program is free software: you can redistribute it and/or modify  *
* it under the terms of the GNU General Public License as published by  *
* the Free Software Foundation, either version 3 of the License, or     *
* (at your option) any later version.                                   *
*                                                                       *
* This program is distributed in the hope that it will be useful,       *
* but WITHOUT ANY WARRANTY; without even the implied warranty of        *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
* GNU General Public License for more details.                          *
*                                                                       *
* You should have received a copy of the GNU General Public License     *
* along with this program.  If not, see <http://www.gnu.org/licenses/>. *
************************************************************************/

///////////////// CONFIG abstract class
class BaseConfig {
public:
  // common
  string config_name;
  string grades_name, serials_name, work_folder;
  Call * callptr;

  // gen
  int exam_number, starting_serial, random_seed;
  string db_folder;
  vector<vector <string>> slot_specs;

  // grade
  string bugs_name, grade_report_basename, results_name;
  int choices_number;
  bool is_call_bugged;

  // correct
  vector<double> thresholds;
  bool make_public_correction;

  // stats
  string stat_report;
  vector<double> bin_pivot;

  // query
  string topics_name;

  BaseConfig() {};
  BaseConfig(string _config_name) : config_name(_config_name) {};
  
  void parsefile(){
    // some std values
    exam_number = -1;
    callptr->score_scale = -1.0;
    starting_serial = -1;
    random_seed = -1;
    choices_number = -1;
    // parse config file
    string key, equal, value;
    ifstream filein(config_name);
    if (!filein) {
      cerr << "Configuration file " << config_name << " not found. Quitting..." << endl;
      exit(3);
    }
    while (filein >> key >> equal >> value) {
      // common
      if (key == "GRADES_FILE") {
        grades_name = value;
      }
      else if (key == "SERIALS") {
        serials_name = value;
      }
      else if (key == "WORK_FOLDER") {
        work_folder = value;
      }
      else if (key == "CALL_NAME") {
        callptr->name = value;
      }
      else if (key == "CALL_DATE") {
        callptr->date = value;
      }
      else if (key == "COURSE") {
        std::getline(filein, callptr->course);
        callptr->course = value + callptr->course;
      }
      else if (key == "TAG") {
        std::getline(filein, callptr->tag);
        callptr->tag = value + callptr->tag;
      }
      else if (key == "CDL") {
        std::getline(filein, callptr->cdl);
        callptr->cdl = value + callptr->cdl;
      }
      else if (key == "COMMISSION") {
        std::getline(filein, callptr->commission);
        callptr->commission = value + callptr->commission;
      }
      // gen
      else if (key == "EXAM_NUMBER") {
        exam_number = stoi(value);
      }
      else if (key.substr(0, 4) == "SLOT") {
        vector<string> specs;
        specs.push_back(value);
        while (true) {
          filein >> value;
          if (value == ";") break;
          specs.push_back(value);
        }
        slot_specs.push_back(specs);
      }
      else if (key == "STARTING_SERIAL") {
        starting_serial = stoi(value);
      }
      else if (key == "RANDOM_SEED") {
        random_seed = stoi(value);
      }
      else if (key == "DATABASE_FOLDER") {
        db_folder = value;
      }
      // grade
      else if (key == "BUGS") {
        bugs_name = value;
      }
      else if (key == "GRADE_REPORT_BASEN") {
        grade_report_basename = value;
      }
      else if (key == "RESULTS") {
        results_name = value;
      }
      else if (key == "CHOICES") {
        choices_number = stoi(value);
      }
      else if (key == "SCORE_SCALE") {
        callptr->score_scale = stod(value);
      }
      // correct
      else if (key == "THRESHOLDS") {
        thresholds.push_back(stod(value));
        while (1) {
          filein >> value;
          if (value == ";") break;
          thresholds.push_back(stod(value));
        }
      }
      // stats
      else if (key == "STATS_REPORT") {
        stat_report = value;
      }
      else if (key == "BINS" && value == "[") {
        while (1) {
          filein >> value;
          if (value == "]") break;
          bin_pivot.push_back(stod(value));
        }
      }
      // query
      else if (key == "TOPICS_FILE") {
        topics_name = value;
      }
      else {
        cerr << "Key " << key << " unknown. Edit " << config_name << endl;
        exit(3);
      }
    }
    filein.close();
  }
  virtual bool check_params() = 0;
};


///////////////// CONFIG for quiz_gen
class GenConfig : public BaseConfig {
public:
  GenConfig(string _config_name, Call * _callptr) {
    config_name = _config_name;
    callptr = _callptr;
    if (config_name == "-conf_t") {
      std::cout << "Creating empty config file \"gen.config\"" << std::endl;
      std::ofstream fileout("gen.config");
      fileout 
        << "CALL_NAME       = call_1" << endl
        << "CALL_DATE       = 1/1/2000" << endl
        << "COURSE          = Course Name" << endl
        << "TAG             = Call 1 - Year 2000" << endl
        << "CDL             = Degree Name" << endl
        << "COMMISSION      = Prof. M. Rossi" << endl
        << "EXAM_NUMBER     = 15" << endl
        << "SLOT1           = regex1.1-* regex1.2-* ;" << endl
        << "[...]" << endl
        << "SLOTn           = regexn.1-* regexn.2-* ;" << endl
        << "STARTING_SERIAL = 1" << endl
        << "RANDOM_SEED     = 1" << endl
        << "SCALE           = 30" << endl
        << "WORK_FOLDER     = call1" << endl
        << "DATABASE_FOLDER = database" << endl << endl;
      fileout.close();
      exit(-1);
    }
  }

  bool check_params() { 
    bool ret = true;
    if (callptr->name == "") {
      cout << "CALL NAME unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->date == "") {
      cout << "CALL DATE unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->course == "") {
      cout << "CALL COURSE unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->tag == "") {
      cout << "CALL TAG unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->cdl == "") {
      cout << "CALL CDL unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->commission == "") {
      cout << "COMMISSION unset. Edit " << config_name << endl;
      ret = false;
    }
    if (exam_number == -1) {
      cout << "EXAM NUMBER unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->score_scale == -1.0) {
      cout << "SCORE SCALE value unset. Edit " << config_name << endl;
      ret = false;
    }
    if (slot_specs.size() == 0) {
      cout << "SPECS unset. Edit " << config_name << endl;
      ret = false;
    }
    else {
      for (auto spec : slot_specs) {
        if (spec.size() == 0) {
          cout << "SLOT_SPEC is empty. Edit " << config_name << endl;
          ret = false;
        }
      }
    }
    if (starting_serial == -1) {
      cout << "STARTING SERIAL unset. Edit " << config_name << endl;
      ret = false;
    }
    if (random_seed == -1) {
      cout << "RANDOM SEED unset. Edit " << config_name << endl;
      ret = false;
    }
    if (work_folder == "") {
      cout << "WORKING FOLDER unset. Edit " << config_name << endl;
      ret = false;
    }
    if (db_folder == "") {
      cout << "DATABASE FOLDER unset. Edit " << config_name << endl;
      ret = false;
    }
    return ret;
  }
};


///////////////// CONFIG for quiz_grade
class GradeConfig : public BaseConfig {
public:
  GradeConfig(string _config_name) {
    config_name = _config_name;
    if (config_name == "-conf_t") {
      std::cout << "Creating empty config file \"grade.config\"" << std::endl;
      std::ofstream fileout("grade.config");
      fileout
        << "BUGS               = errors.txt" << endl
        << "SERIALS            = serials.txt" << endl
        << "GRADE_REPORT_BASEN = call_1" << endl
        << "RESULTS            = results.txt" << endl
        << "GRADES_FILE        = grades.txt" << endl
        << "CHOICES            = 4" << endl
        << "SCORE_SCALE        = 30" << endl
        << "WORK_FOLDER        = call1" << endl << endl;
      fileout.close();
      exit(-1);
    }
  }

  bool check_params() {
    bool ret = true;

    if (bugs_name == "") {
      cout << "BUGS file unset, entering HEALTHY mode" << endl;
      is_call_bugged = false;
    }
    else {
      cout << "BUGS file set, entering BUGGED mode" << endl;
      is_call_bugged = true;
    }
    if (serials_name == "") {
      cout << "SERIALS file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (grade_report_basename == "") {
      cout << "REPORT BASENAME unset. Edit " << config_name << endl;
      ret = false;
    }
    if (results_name == "") {
      cout << "RESULTS file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (grades_name == "") {
      cout << "GRADES file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (choices_number == -1) {
      cout << "CHOICES value unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->score_scale == -1.0) {
      cout << "SCORE SCALE value unset. Edit " << config_name << endl;
      ret = false;
    }
    if (work_folder == "") {
      cout << "WORK FOLDER unset. Edit " << config_name << endl;
      ret = false;
    }

    return ret;
  }
};


///////////////// CONFIG for quiz_correct
class CorrectionConfig : public BaseConfig {
public:
  CorrectionConfig(string _config_name, Call * _callptr) {
    config_name = _config_name;
    callptr = _callptr;
    if (config_name == "-conf_t") {
      std::cout << "Creating empty config file \"correction.config\"" << std::endl;
      std::ofstream fileout("correction.config");
      fileout
        << "CALL_NAME   = call_1" << endl
        << "CALL_DATE   = 1/1/2000" << endl
        << "COURSE      = Course Name" << endl
        << "TAG         = Call 1 - Year 2000" << endl
        << "CDL         = Degree Name" << endl
        << "WORK_FOLDER = call1" << endl
        << "THRESHOLDS  = 16 18 ;" << endl
        << "GRADES_FILE = grades.txt" << endl << endl;
      fileout.close();
      exit(-1);
    }
  }

  bool check_params() {
    bool ret = true;
    if (callptr->name == "") {
      cout << "CALL NAME unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->date == "") {
      cout << "CALL DATE unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->course == "") {
      cout << "CALL COURSE unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->tag == "") {
      cout << "CALL TAG unset. Edit " << config_name << endl;
      ret = false;
    }
    if (callptr->cdl == "") {
      cout << "CALL CDL unset. Edit " << config_name << endl;
      ret = false;
    }
    if (work_folder == "") {
      cout << "WORKING FOLDER unset. Edit " << config_name << endl;
      ret = false;
    }
    if (thresholds.size() == 0) {
      cout << "PUBLIC mode OFF. THRESHOLDS unset." << endl;
      make_public_correction = false;
    }
    else {
      cout << "PUBLIC mode ON. THRESHOLDS set to " << endl;
      cout << "Range 1  ->  [  " << fixed << setprecision(2) << 0.0 << " , " << thresholds[0] << " [ " << endl;
      for (size_t i = 0; i < thresholds.size() - 1; i++) {
        cout << "Range " << i + 2 << "  ->  [ " << fixed << setprecision(2) << thresholds[i] << " , " << thresholds[i + 1] << " [ " << endl;
      }
      cout << "Range " << thresholds.size() + 1 << "  ->  [ " << thresholds.back() << " , " << 30.0 << " ] " << endl << endl;
      make_public_correction = true;
    }
    if (grades_name == "") {
      cout << "GRADES FILE unset. Edit " << config_name << endl;
      ret = false;
    }
    return ret;
  }
};


///////////////// CONFIG for quiz_stats
class StatsConfig : public BaseConfig {
public:
  StatsConfig(string _config_name) {
    config_name = _config_name;
    if (config_name == "-conf_t") {
      std::cout << "Creating empty config file \"stats.config\"" << std::endl;
      std::ofstream fileout("stats.config");
      fileout
        << "WORK_FOLDER  = call1" << endl
        << "GRADES_FILE  = grades.txt" << endl
        << "SERIALS      = serials.txt" << endl
        << "STATS_REPORT = stats.txt" << endl
        << "BINS         = [ 0 10 20 30 ]" << endl << endl;
      fileout.close();
      exit(-1);
    }
  }

  bool check_params() {
    bool ret = true;

    if (work_folder == "") {
      cout << "WORKING folder unset. Edit " << config_name << endl;
      ret = false;
    }
    if (grades_name == "") {
      cout << "GRADES file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (serials_name == "") {
      cout << "SERIALS file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (stat_report == "") {
      cout << "REPORT file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (bin_pivot.size() < 2) {
      cout << "BINS values unset. Edit " << config_name << endl;
      ret = false;
    }

    return ret;
  }
};


///////////////// CONFIG for quiz_query
class QueryConfig : public BaseConfig {
public:
  QueryConfig(string _config_name){
    config_name = _config_name;
    if (config_name == "-conf_t") {
      cout << "Generating empty config file \"query.config\"" << endl;
      std::ofstream config("query.config");
      config
        << "SERIALS     = serials.txt" << endl
        << "GRADES_FILE = voti.txt" << endl
        << "TOPICS_FILE = topics.txt" << endl
        << "WORK_FOLDER = appello1" << endl << endl;
      config.close();
      exit(-1);
    }
  }

  bool check_params() {
    bool ret = true;
    if (grades_name == "") {
      cerr << "GRADES file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (serials_name == "") {
      cerr << "SERIALS file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (topics_name == "") {
      cerr << "TOPICS file unset. Edit " << config_name << endl;
      ret = false;
    }
    if (work_folder == "") {
      cerr << "WORKING folder unset. Edit " << config_name << endl;
      ret = false;
    }
    return ret;
  }
};

