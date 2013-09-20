#ifndef EVENTVIEWER_HPP
#define EVENTVIEWER_HPP
// EventViewer.h
//
// Simple event display class
//
//  Author: R. Beuselinck, Imperial College London
// Created: 20110619
//
#include "GeigerEvent.h"
#include <vector>
class Unpack;
class DriftModel;
class RawGeigerHit;
class TCanvas;
class TEllipse;
class TLine;

class EventViewer
{
  public:
    EventViewer(const DriftModel& m, Unpack& u);
    void show();  // display current event
    void zoom();  // toggle zoomed view.
    bool next();  // read next event (false if EOF reached)
    void clickXY(int, int, int); // handle mouse event in canvas XY.
    void clickXZ(int, int, int); // handle mouse event in canvas XZ.

    friend class ViewerGui; // so it can connect events to the canvas.

  private:
    // implementation methods
    void hilite(double dist, int closest);
    static void hitCoords(const RawGeigerHit& h, double& x, double& y,
      double& z, int& updown);

    // data members
    const DriftModel&   _drift;
    Unpack&  _unp;
    GeigerEvent _event;
    TCanvas* _xyCanvas;
    TCanvas* _xzCanvas;
    bool _isZoomed;  // flag for zoom state.
    double _xl, _xh, _yl, _yh, _zl, _zh;  // range for zoom views

    // containers for hit association to ROOT primitives
    //
    std::vector<GeigerEvent::HitList::const_iterator> _hitToRoot;
    std::vector<TEllipse*> _xyhit;
    std::vector<TLine*> _xzhit;
};

#endif    /* EVENTVIEWER_HPP */
