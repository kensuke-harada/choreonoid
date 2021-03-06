/**
  @author Shin'ichiro Nakaoka
*/

#include "../GrxUIPlugin.h"
#include "../GrxUIMenuView.h"
#include <cnoid/LazyCaller>

using namespace cnoid;
namespace python = boost::python;

namespace {

    void checkGrxUIPlugin()
    {
        if(!GrxUIPlugin::isActive()){
            PyErr_SetString(PyExc_RuntimeError, "GrxUI Plugin is inactive.");
            python::throw_error_already_set();
        }
    }

    python::object cancelExceptionType;
    
    void waitInputMenu(const python::list& menu)
    {
        checkGrxUIPlugin();
        callSynchronously([&](){ GrxUIMenuView::instance()->setMenu(menu, false, !isRunningInMainThread()); });
    }

    void waitInputSequentialMenu(const python::list& menu)
    {
        checkGrxUIPlugin();
        callSynchronously([&](){ GrxUIMenuView::instance()->setMenu(menu, true, !isRunningInMainThread()); });
    }

    void waitInputSelectMain(const std::string& message, QMessageBox::StandardButton& out_result)
    {
        if(GrxUIPlugin::isActive()){
            out_result = GrxUIMenuView::waitInputSelect(message);
        }
    }
            
    bool waitInputSelect(const std::string& message)
    {
        checkGrxUIPlugin();
        QMessageBox::StandardButton result = QMessageBox::Cancel;

        Py_BEGIN_ALLOW_THREADS        
        callSynchronously([&](){ waitInputSelectMain(message, result); });
        Py_END_ALLOW_THREADS
        
        if(result == QMessageBox::Cancel){
            PyErr_SetObject(cancelExceptionType.ptr(), 0);
            python::throw_error_already_set();
        }
        return (result == QMessageBox::Yes);
    }

    void waitInputConfirmMain(const std::string& message, bool& out_result)
    {
        if(GrxUIPlugin::isActive()){
            out_result = GrxUIMenuView::waitInputConfirm(message);
        }
    }

    bool waitInputConfirm(const std::string& message)
    {
        checkGrxUIPlugin();

        bool result = false;

        Py_BEGIN_ALLOW_THREADS        
        callSynchronously([&](){ waitInputConfirmMain(message, result); });
        Py_END_ALLOW_THREADS
            
        if(!result){
            PyErr_SetObject(cancelExceptionType.ptr(), 0);
            python::throw_error_already_set();
        }

        return true;
    }

    void waitInputMessageMain(const std::string& message, std::string& out_result)
    {
        if(GrxUIPlugin::isActive()){
            out_result = GrxUIMenuView::waitInputMessage(message);
        }
    }

    std::string waitInputMessage(const std::string& message)
    {
        checkGrxUIPlugin();

        std::string result;

        Py_BEGIN_ALLOW_THREADS        
        callSynchronously([&](){ waitInputMessageMain(message, result); });
        Py_END_ALLOW_THREADS

        /*
        if(!result){
            PyErr_SetObject(cancelExceptionType.ptr(), 0);
            python::throw_error_already_set();
        }
        */

       return result;
    }
}


BOOST_PYTHON_MODULE(grxui)
{
    if(!GrxUIPlugin::isActive()){
        PyErr_SetString(PyExc_ImportError, "GrxUI Plugin is not loaded.");
        python::throw_error_already_set();
    }
    
    python::def("waitInputMenu", waitInputMenu);
    python::def("waitInputSequentialMenu", waitInputSequentialMenu);
    python::def("waitInputSelect", waitInputSelect);
    python::def("waitInputConfirm", waitInputConfirm);
    python::def("waitInputMessage", waitInputMessage);

    // define the GrxUICancelException class which inherits the built-in Exception class
    python::object mainModule = python::import("__main__");
    python::object mainName = mainModule.attr("__name__");
    mainModule.attr("__name__") = python::scope().attr("__name__");
    python::exec("class GrxUICancelException (RuntimeError): pass\n",
                 mainModule.attr("__dict__"), python::scope().attr("__dict__"));
    cancelExceptionType = python::scope().attr("GrxUICancelException");
    GrxUIMenuView::setCancelExceptionType(cancelExceptionType);
    mainModule.attr("__name__") = mainName;
}
