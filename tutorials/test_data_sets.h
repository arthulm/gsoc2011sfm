
#ifndef SFM_TEST_DATA_SETS_H_
#define SFM_TEST_DATA_SETS_H_

#include "config.h"
#include "../src/PointOfView.h"
#include "../src/CameraPinhole.h"
#include <opencv2/core/core.hpp>

#include <iostream>
#include <fstream>
#include <vector>
#include <string>


//Should not use "using namespace" but as we are in
//tutorial section, this will help readability
using namespace std;
using namespace OpencvSfM;
using namespace cv;

// Use this annotation at the end of a struct/class definition to
// prevent the compiler from optimizing away instances that are never
// used.
#if defined(__GNUC__) && !defined(COMPILER_ICC)
# define _ATTRIBUTE_UNUSED_ __attribute__ ((unused))
#else
# define _ATTRIBUTE_UNUSED_
#endif
// A macro to disallow operator=
// This should be used in the private: declarations for a class.
#define _DISALLOW_ASSIGN_(type)\
  void operator=(type const &)

// A macro to disallow copy constructor and operator=
// This should be used in the private: declarations for a class.
#define _DISALLOW_COPY_AND_ASSIGN_(type)\
  type(type const &);\
  _DISALLOW_ASSIGN_(type)

class Tutorial_Handler;

// Defines the abstract factory interface that creates instances
// of a Test object.
class TutoFactoryBase {
 public:
  virtual ~TutoFactoryBase() {}

  // Creates a test instance to run. The instance is both created and destroyed
  // within TestInfoImpl::Run()
  virtual Tutorial_Handler* CreateTest() = 0;

 protected:
  TutoFactoryBase(string n,string h,string f)
  {name=n;help=h;file=f;}
  string name;
  string help;
  string file;

 private:
  _DISALLOW_COPY_AND_ASSIGN_(TutoFactoryBase);
};

class Tutorial_Handler;
class Intern_tutorial_list
{
  vector<Tutorial_Handler*> list_of_tutos;
public:
  static Intern_tutorial_list *getInstance()
  {
    static Intern_tutorial_list *ref_static = 
      new Intern_tutorial_list();
    return ref_static;
  }
  friend class Tutorial_Handler;
};

// This class create an instance of tutorial...
template <class TutoClass>
class TutoFactoryImpl : public TutoFactoryBase {
 public:
   TutoFactoryImpl(string n,string h,string f)
     :TutoFactoryBase(n,h,f){};
  virtual Tutorial_Handler* CreateTest() {
    return new TutoClass(name,help,file); }
};

CREATE_EXTERN_MUTEX( my_mutex_Tutorial_Handler );

class Tutorial_Handler
{
  _DISALLOW_COPY_AND_ASSIGN_(Tutorial_Handler);
protected:
  Tutorial_Handler(string name,string help,string file)
  {
    this->name_of_tuto = name;
    this->tuto_help = help;
    this->file_of_tuto = file;
  }

  virtual void tuto_body()=0;

public:
  
  template <class TutoClass>
  static Tutorial_Handler* registerTuto(TutoFactoryImpl<TutoClass>* addTuto)
  {
    Tutorial_Handler* out = addTuto->CreateTest();
    P_MUTEX( my_mutex_Tutorial_Handler );
    vector<Tutorial_Handler*> &local_list_of_tutos =
      Intern_tutorial_list::getInstance()->list_of_tutos;
    local_list_of_tutos.push_back(out);
    V_MUTEX( my_mutex_Tutorial_Handler );
    return out;
  }


  static int print_menu();

  static void run_tuto(int id_tuto);
  
  string name_of_tuto;
  string file_of_tuto;
  string tuto_help;
};


//////////////////////////////////////////////////////////////////////////
//Only to see how we can create a 3D structure estimation using calibrated cameras
//////////////////////////////////////////////////////////////////////////
enum { LOAD_INTRA=1, LOAD_POSITION=2, LOAD_FULL=3};

vector<PointOfView> loadCamerasFromFile(string fileName, int flag_model = LOAD_FULL);



#define TUTO_CLASS_NAME_(tuto_name) \
  class_##tuto_name##_Test
// Helper macro for defining tuto.
#define NEW_TUTO(t_name, tuto_name, tuto_help)\
class TUTO_CLASS_NAME_(t_name) : public Tutorial_Handler {\
 public:\
  TUTO_CLASS_NAME_(t_name)(string n,string h,string f)\
    :Tutorial_Handler(n,h,f){}\
  static Tutorial_Handler* const add_to_vector _ATTRIBUTE_UNUSED_;\
 private:\
  _DISALLOW_COPY_AND_ASSIGN_( TUTO_CLASS_NAME_(t_name) );\
  virtual void tuto_body();\
};\
Tutorial_Handler* const TUTO_CLASS_NAME_(t_name)\
  ::add_to_vector = Tutorial_Handler\
    ::registerTuto(new TutoFactoryImpl<TUTO_CLASS_NAME_(t_name)>(tuto_name, tuto_help,\
    __FILE__));\
void TUTO_CLASS_NAME_(t_name)::tuto_body()



#endif  // LIBMV_MULTIVIEW_TEST_DATA_SETS_H_