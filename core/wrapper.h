#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include "pybind11/pybind11.h"
#include "smallquery.h"

PYBIND11_MODULE(_smallquery, m) {
    namespace py = pybind11;
    py::class_<SmallQuery>(m, "SmallQuery")
        .def(py::init<>())
        .def("create_table", &SmallQuery::CreateTable)
        .def("execute", &SmallQuery::Execute);
}

#endif