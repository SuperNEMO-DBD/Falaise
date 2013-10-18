// EventViewer.cc
//
#include "EventViewer.h"
#include "ViewerGui.h"
#include "DriftModel.h"
#include "Unpack.h"
#include "GeigerEvent.h"
#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TLine.h>
#include <TEllipse.h>
#include <iostream>
#include <string>
#include <vector>

EventViewer::EventViewer(const DriftModel& m, Unpack& u)
: _drift(m), _unp(u)
{
  std::cout << "Initialising EventViewer" << std::endl;
  int argc = 0;
  char **argv = 0;
  TApplication app("app", &argc, argv);
  _xyCanvas = new TCanvas("xy", "Event Display (X-Y)", 500, 500);
  _xyCanvas->Range(-250., 0., 250., 500.);
  _xzCanvas = new TCanvas("xz", "Event Display (X-Z)", 500, 500);
  _xzCanvas->Range(-150., -10., 150., 290.);
  ViewerGui g(this);
  if (next()) show(); // display the first event.
  app.Run(true);
}

bool EventViewer::next()
{
  static char bell = 0x7;
  Unpack::status ret = Unpack::OK;
  long evno = -1;
  ret = _unp.getNextEvent(_event);
  if (ret == Unpack::END) return false;

  evno = _event.evtNumber();
  if (ret != Unpack::OK)
  {
    std::cout << bell << "WARNING: Bad status from event " << evno << std::endl;
    const UnpackingErrors err = _unp.getErrors();
    std::cout << "There are " << err.size() << " errors recorded" << std::endl;
    const UnpackingErrors::ErrorList el = err.getErrors();
    UnpackingErrors::ErrorList::const_iterator it = el.begin();
    for ( ; it != el.end(); ++it)
    {
      std::cout << "Error code: " << it->first << '\n'
        << "Error text: " << it->second;
    }
  }
  return true;
}

void EventViewer::clickXY(int ev, int px, int py)
{
  if (ev != 1) return;  // button1 down
  double x = _xyCanvas->PixeltoX(px);
  double y = _xyCanvas->PixeltoY(py - _xyCanvas->GetWh());  // don't ask!

  // Find the closest primitive to the mouse event.
  //
  double d2min = 999.;
  int closest = 0;
  for (int i=0; i<_xyhit.size(); ++i)
  {
    TEllipse* e = _xyhit[i];
    double dx = x - e->GetX1();
    double dy = y - e->GetY1();
    double d2 = dx*dx + dy*dy;
    if (d2 < d2min)
    {
      d2min = d2;
      closest = i;
    }
  }
  hilite(d2min, closest);
}

void EventViewer::clickXZ(int ev, int px, int pz)
{
  if (ev != 1) return;  // button1 down
  double x = _xzCanvas->PixeltoX(px);
  double y = _xzCanvas->PixeltoY(pz - _xzCanvas->GetWh());  // don't ask!

  // Find the closest primitive to the mouse event.
  //
  double d2min = 999.;
  int closest = 0;
  for (int i=0; i<_xzhit.size(); ++i)
  {
    TLine* l = _xzhit[i];
    double dx = x - 0.5*(l->GetX1() + l->GetX2());
    double dy = y - l->GetY1();
    double d2 = dx*dx + dy*dy;
    if (d2 < d2min)
    {
      d2min = d2;
      closest = i;
    }
  }
  hilite(d2min, closest);
}

void EventViewer::hilite(double dist, int closest)
{
  // Hightlight the primitive index by closest in both views if we are
  // within dist.
  //
  // Cancel any highlighting if we clicked too far from any primitive
  //
  if (dist > 5.)
  {
    std::cout << "Nothing selected" << std::endl;
    for (int i=0; i<_xyhit.size(); ++i)
    {
      _xyhit[i]->SetLineWidth(1);
      _xzhit[i]->SetLineWidth(1);
    }
    _xyCanvas->Modified();
    _xyCanvas->Update();
    _xzCanvas->Modified();
    _xzCanvas->Update();
    return;
  }

  // Highlight the closest primitive in both views.
  //
  TEllipse* e = _xyhit[closest];
  TLine* l = _xzhit[closest];
  GeigerEvent::HitList::const_iterator it = _hitToRoot[closest];
  const RawGeigerHit& gh = *it;
  std::cout << gh;
  std::cout << "Drift circle:\n"
  << "centre X: " << e->GetX1() << '\n'
  << "centre Y: " << e->GetY1() << '\n'
  << "radius:   " << e->GetR1() << '\n'
  << "Z coord:  " << l->GetY1() << std::endl;
  e->SetLineWidth(2);
  _xyCanvas->Modified();
  _xyCanvas->Update();
  l->SetLineWidth(2);
  _xzCanvas->Modified();
  _xzCanvas->Update();
}

void EventViewer::show()
{
  TLine *xlo = new TLine(-50., 0., -50., 500.);
  TLine *xhi = new TLine(+50., 0., +50., 500.);
  TLine *foilxy = new TLine(0., 0., 0., 500.);
  TLine *foilxz = new TLine(0., 0., 0., 290.);
  xlo->SetLineWidth(2);
  xhi->SetLineWidth(2);
  foilxy->SetLineColor(kRed);
  foilxz->SetLineColor(kRed);
  xlo->Draw();
  xhi->Draw();
  foilxy->Draw();
  _xzCanvas->Clear();
  _xzCanvas->Range(-150., -10., 150., 290.);
  xlo->Draw();
  xhi->Draw();
  foilxz->Draw();
  _xyCanvas->Clear();
  _xyCanvas->Range(-250., 0., 250., 500.);
  xlo->Draw();
  xhi->Draw();
  foilxy->Draw();
  std::cout << "Display event # " << _event.evtNumber() << std::endl;
  const GeigerEvent::HitList& hits = _event.getHits();
  GeigerEvent::HitList::const_iterator it = hits.begin();
  std::cout << "   x     y     dist   z   updown\n"
            << "--------------------------------" << std::endl;
  _xl = 999.;
  _xh = -999.;
  _yl = 999.;
  _yh = -999.;
  _zl = 999.;
  _zh = -999.;

  // create a cross-reference between event data and drawn primitives
  //
  int nh = hits.size();
  std::vector<GeigerEvent::HitList::const_iterator> hitIds;
  std::vector<TEllipse*> ellipses;
  std::vector<TLine*> lines;
  hitIds.reserve(nh);
  ellipses.reserve(nh);
  lines.reserve(nh);
  int fID = 0;
  for ( ; it != hits.end(); ++it)
  {
    double x, y, r, z;
    int ud;
    hitCoords(*it, x, y, z, ud);
    r = _drift.getDist(*it);
    std::cout << x << ' ' << y << ' ' << r << ' '
      << z << ' ' << ud << std::endl;
    TEllipse* e = new TEllipse(x, y, r, r);
    e->SetUniqueID(fID);
    if (ud == 1) e->SetLineColor(kBlue);        // determined from top only
    else if (ud == -1) e->SetLineColor(kGreen); // determined from bot only
    else if (ud == -99) e->SetLineColor(kRed);  // no z information at all.
    e->Draw();

    // calculate the maximum range of points
    //
    if (x < _xl) _xl = x;
    if (x > _xh) _xh = x;
    if (y < _yl) _yl = y;
    if (y > _yh) _yh = y;
    if (z < _zl && z > -10.) _zl = z; // don't distort with no-info points.
    if (z > _zh) _zh = z;

    // draw the x-z view
    //
    _xzCanvas->cd();
    double dx = 0.5*r;
    TLine* l = new TLine(x-dx, z, x+dx, z);
    l->SetUniqueID(fID);
    if (ud == 1) l->SetLineColor(kBlue);        // determined from top only
    else if (ud == -1) l->SetLineColor(kGreen); // determined from bot only
    else if (ud == -99) l->SetLineColor(kRed);  // this should be off the plot!
    l->Draw();
    _xyCanvas->cd();

    // link the primitives to the hit.
    //
    hitIds.push_back(it);
    ellipses.push_back(e);
    lines.push_back(l);
    fID++;
  }
  _hitToRoot.swap(hitIds);
  _xyhit.swap(ellipses);
  _xzhit.swap(lines);

  // adjust the zoom range to preserve aspect ratio and provide a border
  //
  _isZoomed = false;
  _xl -= 10.;
  _xh += 10.;
  _yl -= 10.;
  _yh += 10.;
  _zl -= 10.;
  _zh += 10.;
  double dx = _xh - _xl;
  double dy = _yh - _yl;
  double dz = _zh - _zl;
  if (dx > dy)
  {
    double d2 = 0.5*(dx - dy);
    _yl -= d2;
    _yh += d2;
  }
  else if (dy > dx)
  {
    double d2 = 0.5*(dy - dx);
    _xl -= d2;
    _xh += d2;
  }
  if (dx > dz)
  {
    double d2 = 0.5*(dx - dz);
    _zl -= d2;
    _zh += d2;
  }
  _xyCanvas->Update();
  _xzCanvas->Update();
}

void EventViewer::zoom()
{
  // zoom current view (toggle)
  //
  _isZoomed = !_isZoomed;
  std::cout << "Zoom set to " << _isZoomed << std::endl;
  if (_isZoomed)
  {
    _xyCanvas->Range(_xl, _yl, _xh, _yh);
    _xzCanvas->Range(_xl, _zl, _xh, _zh);
    std::cout << "Range " << ' ' <<_xl << ' ' <<_yl
    << ' ' <<_xh << ' ' <<_yh << ' ' << _zl << ' ' << _zh << std::endl;
  }
  else
  {
    _xyCanvas->Range(-250., 0., 250., 500.);
    _xzCanvas->Range(-150., -10., 150., 290.);
    std::cout << "Range default" << std::endl;
  }
  _xyCanvas->Modified();
  _xyCanvas->Update();
  _xzCanvas->Modified();
  _xzCanvas->Update();
}

void EventViewer::hitCoords(const RawGeigerHit& h, double& x, double& y,
  double& z, int& updown)
{
  // Calculate the physical location of a Geiger cell. (distances in cm).
  //
  const double pitch = 4.4;    // cell and anode wire pitch
  const double xoff = 1.4;     // offset from X wall (y=0)
  const double yoff = 1.0;     // offset from calorimeter wall (x=0)
  const double height = 280.;  // distance between top and bottom cathodes.
  const int cattdcmax = 46666; // empirical!

  // Source foil lies in the y-z plane
  //
  int module = h.module();  // ignoring this, always 0 for demonstrator.
  int side = h.side();      // 0 = -ve X, 1 = +ve X.
  int layer = h.layer();    // X index, 0 to 8.
  int column = h.column();  // Y index, 0 to 112.

  x = (layer + 0.5)*pitch + xoff;
  if (side == 0) x = -x;
  y = (column + 0.5)*pitch + yoff;

  // Calculate the z coordinate based on the top and bottom cathode tdc times
  // (assumed linear relation). Infer scale from cattdcmax if one of the tdc
  // values is missing.
  //
  int top = h.cathodeTopTDC();
  int bot = h.cathodeBotTDC();
  if (top+bot == 0)
  {
    updown = -99; // no data
    z = -99.;
  }
  else if (top == 0)
  {
    updown = -1;
    z = (height*bot)/cattdcmax;
  }
  else if (bot == 0)
  {
    updown = 1;
    z = height - (height*top)/cattdcmax;
  }
  else
  {
    updown = 0;
    z = (height*bot)/(top+bot);
  }
}
