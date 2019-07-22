/* -*- mode: c++ -*- */
// Header file defining a simple messenger class
/*
 *
 * J.J. Gomez-Cadenas, J.A. Hernando, A. Cervera, A. Tornero
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#ifndef __MESSENGER__
#define __MESSENGER__

#include <string>
#include <mybhep/utilities.h>
#include <iostream>
#include <fstream>

namespace mybhep {
/// Simple messenger class
/**
 * It sends messages conditioned to the level of information requested
 *at construction time or any time later via the set_level method. The
 *message structure is:
 * message(string,Ta,Tb,Tc....,LEVEL) where Ta, Tb,Tc... are templated types
 *and level is the level of information for which this message will be output
 *\ingroup base
 */
class messenger {
 private:
  prlevel level_;

 public:
  /// Default constructor is set to MUTE
  messenger() { level_ = MUTE; };
  /// Takes the prlevel as an argument
  messenger(prlevel clevel) { level_ = clevel; }
  /// Sets the print level
  void set_level(prlevel clevel) { level_ = clevel; }
  /// Returns the print level
  prlevel level() const { return level_; }
  /// Sends a message
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger. For example, if the messenger is set to VVERBOSE
   * any message will be printed. Instead, if the messenger is set to MUTE
   * only messages flagged as MUTE will print
   *\ingroup base
   */
  inline void message(const std::string& the_message, prlevel clevel) const {
    if (clevel <= level_) std::clog << the_message << std::endl;
  }
  /// Sends a message followed by variable d
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class T>
  inline void message(const std::string& the_message, const T& d, prlevel clevel) const {
    if (clevel <= level_) std::clog << the_message << " " << d << std::endl;
  }

  /// Sends a message followed by variable d1 and d2
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B>
  inline void message(const std::string& the_message, const A& d1, const B& d2,
                      prlevel clevel) const {
    if (clevel <= level_) std::clog << the_message << " " << d1 << " " << d2 << std::endl;
  }
  /// Sends a message followed by variable d1 d2 and d3
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << std::endl;
  }
  /// Sends a message followed by variable d1 d2, d3 and d4
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << std::endl;
  }
  /// Sends a message followed by variable d1 d2, d3 d4 and d5
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D, class E>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, const E& d5, prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << " " << d5
                << std::endl;
  }

  /// Sends a message followed by variable d1 d2, d3 d4, d5, and d6
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D, class E, class F>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, const E& d5, const F& d6, prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << " " << d5
                << " " << d6 << std::endl;
  }
  /// Sends a message followed by variable d1 d2, d3 d4, d5, and d6 d7
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D, class E, class F, class G>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, const E& d5, const F& d6, const G& d7, prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << " " << d5
                << " " << d6 << " " << d7 << std::endl;
  }
  /// Sends a message followed by variable d1 d2, d3 d4, d5, d7 d8
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D, class E, class F, class G, class H>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, const E& d5, const F& d6, const G& d7, const H& d8,
                      prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << " " << d5
                << " " << d6 << " " << d7 << " " << d8 << std::endl;
  }
  /// Sends a message followed by variable d1 d2, d3 d4, d5, d7 d8 d9
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D, class E, class F, class G, class H, class I>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, const E& d5, const F& d6, const G& d7, const H& d8, const I& d9,
                      prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << " " << d5
                << " " << d6 << " " << d7 << " " << d8 << " " << d9 << std::endl;
  }
  /// Sends a message followed by variable d1 d2, d3 d4, d5, d7 d8 d9 d10
  /** The specified print level must be equal or smaller than the print level
   * set to the messenger.
   *\ingroup base
   */
  template <class A, class B, class C, class D, class E, class F, class G, class H, class I,
            class J>
  inline void message(const std::string& the_message, const A& d1, const B& d2, const C& d3,
                      const D& d4, const E& d5, const F& d6, const G& d7, const H& d8, const I& d9,
                      const J& d10, prlevel clevel) const {
    if (clevel <= level_)
      std::clog << the_message << " " << d1 << " " << d2 << " " << d3 << " " << d4 << " " << d5
                << " " << d6 << " " << d7 << " " << d8 << " " << d9 << " " << d10 << std::endl;
  }
};
}  // namespace mybhep

#endif
