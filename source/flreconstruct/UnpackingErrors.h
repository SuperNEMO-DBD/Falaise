#ifndef UNPACKINGERRORS_HPP
#define UNPACKINGERRORS_HPP
// UnpackingErrors.h
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110512
//
// A collection of errors found during raw data unpacking.
//
#include <list>
#include <utility>
#include <string>

class UnpackingErrors
{
  public:
    typedef std::pair<int, std::string> anError;
    typedef std::list<anError>  ErrorList;

    UnpackingErrors() { _errors.clear(); }

    void add(int code, const std::string& s);
    void clear() { _errors.clear(); }
    int size() const { return _errors.size(); }
    const ErrorList& getErrors() const { return _errors; }

  private:
    ErrorList _errors;
};

#endif    /* UNPACKINGERRORS_HPP */
