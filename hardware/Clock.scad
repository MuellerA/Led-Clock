////////////////////////////////////////////////////////////////////////////////
// Clock
////////////////////////////////////////////////////////////////////////////////
//
// (c) Andreas Mueller
// License: Creative Commons - Attribution
// http://creativecommons.org/licenses/by/4.0/
//
////////////////////////////////////////////////////////////////////////////////

// radius of clock
radius_outer   = 120   ; // [50:5:200]
// radius of hole in the middle
radius_inner   =  40   ; // [0:5:150]
// width of bottom, outer and inner wall
width          =   1.6 ; // [0.8:0.1:5]
// width of wall between beams
spacer_width   =   0.8 ; // [0.4:0.1:5]
// height of beam box
height         =  13   ; // [8:1:30]

// number of LEDs/beams
ws_count       = 60   ; // [3:1:100]
// radius of LED PCB
ws_hole_radius =  5   ; // [3:0.5:7]
// size of LED
ws_size        =  5.5 ; // [3:0.5:7]

// height of wire channel
wire_height    =  1.6 ; // [0:0.2:3]
// width of wire channel
wire_width     =  3   ; // [0:0.2:5]

// split clock in parts
parts          =  1   ; // [1:one, 2:two, 4:four]

/* [Hidden] */

PI = 3.141592654 ;
$fa =  6 ;
$fs =  1 ;

////////////////////////////////////////////////////////////////////////////////

module ws()
{
  // outer
  rO = radius_outer - width ;
  uO = 2 * PI * rO ;
  wO = uO / ws_count - spacer_width ;

  // inner
  uMin = spacer_width * ws_count ;
  rMin = uMin / 2 / PI ;

  rI = (rMin > radius_inner + width) ? rMin : (radius_inner + width) ;
  uI = 2 * PI * rI ;
  wI = uI / ws_count - spacer_width ;

  hO = ws_hole_radius / radius_outer * (rO) ;
  hI = ws_hole_radius / radius_outer * (rI) ;

  da = 360 / ws_count ;

  // corona
  intersection()
  {
    for (a = [da/2:da:360])
    rotate([0, 0, a])
    {
      translate([0, 0, ws_hole_radius])
      rotate([90, 0, 0])
      hull()
      {
        translate([0, 0, rO])
        union()
        {
          translate([0, height, 0]) cube([wO, 2*height, 0.01], center = true) ;
          scale([wO/2, hO, 1])
          cylinder(r = 1, h = 0.01, center = true, $fn=30) ;
        }
        
        translate([0, height-ws_hole_radius, rI])
        union()
        {
          translate([0, height/2, 0]) cube([wI, height, 0.01], center = true) ;
          scale([wI/2, hI, 1])
          cylinder(r = 1, h = 0.01, center = true, $fn=30) ;
        }
      }
    }

    cylinder(r = rO, h = 4*(height+width)) ;
  }
  
  // hole for LED
  for (a = [da/2:da:360])
  rotate([0, 0, a])
  {
    translate([0, -radius_outer, height/2])
    cube([ws_size, 4*width, ws_size], center=true) ;
  }
}

////////////////////////////////////////////////////////////////////////////////

module wire()
{
  da = 360 / ws_count ;

  for (a = [da:90:360])
  rotate([0, 0, -a])
  translate([0, 0, wire_height/2]) cube([3*(radius_outer), wire_width, wire_height], center=true) ;
}

////////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////////

difference ()
{
  translate([0, 0, (height+width)/2]) cylinder(r = radius_outer, h = height + width, center = true) ;
  translate([0, 0, (height+width)/2]) cylinder(r = radius_inner, h = 2 * (height + width), center = true) ;

  translate([0, 0, width]) ws() ;
  wire() ;

  if (parts == 4)
  {
    translate([2*radius_outer, 0, (height+width)/2]) cube([4*radius_outer, 4*radius_outer, 2*(height + width)], center = true) ;
    translate([0, 2*radius_outer, (height+width)/2]) cube([4*radius_outer, 4*radius_outer, 2*(height + width)], center = true) ;
  }
  if ((parts == 4) || (parts == 2))
  {
    translate([2*radius_outer, 0, (height+width)/2]) cube([4*radius_outer, 4*radius_outer, 2*(height + width)], center = true) ;
  }
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
