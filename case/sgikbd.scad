/* Case for the SGI PS/2 keyboard/mouse converter board
 * by John Tsiombikas <nuclear@mutantstargoat.com>
 *
 * project website: http://nuclear.mutantstargoat.com/hw/sgikbd
 * github repo: https://github.com/jtsiomb/sgikbd
 *
 * This case design is in the public domain, use and modify as you will.
 */

// --------------------- Options ------------------------------
// breaktabs: break-away panels for unpopulated output ports
breaktabs = true;

// no_db15: solid wall where the DB15 port would be
no_db15 = false;
// no_db9: solid wall where the IRIS 4D series DB9 port would be
no_db9 = false;
// no_minidin: solid wall where the Indigo/Onyx mini-din port would be
no_minidin = false;

// include top part of the case
have_top = true;
// include bottom part of the case
have_bottom = true;
// ------------------------------------------------------------


pcb_width = 80;
pcb_height = 60;
pcb_thick = 1.8;
pcb_bev = 4;
holeoffs = 4.5;
hole_d = 3.2;
thick = 1.5;

ps2_h = 13.8;
ps2_w = 14.5;
inp_offs = 20;

db9_w = 32;
db9_h = 14;
db9_offs = 55.2;
db15_w = 40;
db15_h = db9_h;
db15_offs = 50.95;

overhead = db9_h;

stand_h = 3;
stand_d = hole_d * 2;

gap = 0.75;		// total gap between board and case

break_gap = 0.8;


module pcb()
{
	translate([pcb_bev, pcb_bev, -pcb_thick])
	linear_extrude(pcb_thick) {
		minkowski() {
			square([pcb_width-pcb_bev*2, pcb_height-pcb_bev*2]);
			circle(pcb_bev, $fn=40);
		}
	}
}

module ps2port()
{
	translate([-ps2_w/2, -10, 0])
	cube([ps2_w, 20, ps2_h]);
}

module indigoport()
{
	difference() {
		translate([-ps2_w/2, -pcb_width/2+thick*2, 0])
		cube([ps2_w, pcb_width/2, ps2_h]);
		if(breaktabs) {
			translate([-ps2_w/2+break_gap, -thick, 0])
			cube([ps2_w-break_gap*2, thick*3, ps2_h-break_gap]);
			translate([-ps2_w/2-break_gap, 0, ps2_h/2])
			cube([ps2_w+break_gap*2, thick*3, 0.4]);
			
			d = (ps2_w-1-break_gap*2) / 4;
			for(i=[0:d:ps2_w-break_gap*2]) {
				translate([-ps2_w/2+break_gap+i, 0, ps2_h-break_gap-0.5])
				cube([1, thick*3, break_gap+1]);
			}
		}
	}
}

module db9port()
{
	difference() {
		translate([-db9_w/2, -pcb_height/2+thick*2, 0])
		cube([db9_w, pcb_height/2, db9_h]);
		if(breaktabs) {
			translate([-db9_w/2+break_gap, -thick, 0])
			cube([db9_w-break_gap*2, thick*3, db9_h-break_gap]);
			translate([-db9_w/2-break_gap, 0, db9_h/2])
			cube([db9_w+break_gap*2, thick*3, 0.4]);
			
			d = (db9_w-1-break_gap*2) / 10;
			for(i=[0:d:db9_w-2]) {
				translate([-db9_w/2+break_gap+i, 0, db9_h-break_gap-0.5])
				cube([1, thick*3, break_gap+1]);
			}
		}
	}
}

module db15port()
{
	difference() {
		translate([-db15_w/2, -pcb_height/2+thick*2, 0])
		cube([db15_w, pcb_height/2, db15_h]);
		if(breaktabs) {
			translate([-db15_w/2+break_gap, -thick, 0])
			cube([db15_w-break_gap*2, thick*3, db15_h-break_gap]);
			translate([-db15_w/2-break_gap, 0, db15_h/2])
			cube([db15_w+break_gap*2, thick*3, 0.4]);
			
			d = (db15_w-1-break_gap*2) / 13;
			for(i=[0:d:db15_w-2]) {
				translate([-db15_w/2+break_gap+i, 0, db15_h-break_gap-0.5])
				cube([1, thick*3, break_gap+1]);
			}
		}
	}
}

module board()
{
	pcb();
	
	translate([0, inp_offs, 0])
	rotate([0, 0, 90])
	ps2port();
	
	translate([0, pcb_height-inp_offs, 0])
	rotate([0, 0, 90])
	ps2port();
	
	if(!no_minidin) {
		translate([pcb_width, pcb_height/2, 0])
		rotate([0, 0, 270])
		indigoport();
	}
	
	if(!no_db9) {
		translate([db9_offs, pcb_height, 0])
		db9port();
	}
	
	if(!no_db15) {
		translate([db15_offs, 0, 0])
		rotate([0, 0, 180])
		db15port();
	}
}

module case_shape(w, h, bev)
{
	translate([bev, bev, 0])
	minkowski() {
		square([w-bev*2, h-bev*2]);
		circle(bev, $fn=80);
	}
}

module standoff()
{
	cylinder(stand_h, d1=stand_d*1.5, d2=stand_d, $fn=60);
}

module screwhole()
{
	translate([0, 0, -0.5-thick])
	cylinder(stand_h + thick + 1, d=hole_d, $fn=40);
	
	// screw head
	translate([0, 0, -0.1-thick])
	cylinder(3, d=6.8, $fn=50);
	
	translate([0, 0, stand_h])
	cylinder(pcb_thick+overhead-thick, d=2.82, $fn=30);
}

module case_bot()
{
	h = stand_h + thick + pcb_thick;

	difference() {
		union() {
			translate([-thick-gap/2, -thick-gap/2, -h])
			difference() {
				linear_extrude(h)
				case_shape(pcb_width + thick * 2 + gap, pcb_height + thick * 2 + gap, holeoffs);
				
				translate([thick, thick, thick])
				linear_extrude(h)
				case_shape(pcb_width + gap, pcb_height + gap, holeoffs);
			}
			
			for(i=[0:1]) {
				x = holeoffs + i * (pcb_width - holeoffs*2);
				for(j=[0:1]) {
					y = holeoffs + j * (pcb_height - holeoffs*2);
					translate([x, y, -stand_h-pcb_thick])
					standoff();
				}
			}
		}
		for(i=[0:1]) {
			x = holeoffs + i * (pcb_width - holeoffs*2);
			for(j=[0:1]) {
				y = holeoffs + j * (pcb_height - holeoffs*2);
				translate([x, y, -stand_h-pcb_thick])
				screwhole();
			}
		}
	}
}

module case_top()
{
	difference() {
		union() {
			translate([-thick-gap/2, -thick-gap/2, 0])
			difference() {
				linear_extrude(overhead+thick)
				case_shape(pcb_width + thick * 2 + gap, pcb_height + thick * 2 + gap, holeoffs);
			
				translate([thick, thick, -thick])
				linear_extrude(overhead+thick)
				case_shape(pcb_width + gap, pcb_height + gap, holeoffs);
			}
	
			for(i=[0:1]) {
				x = holeoffs + i * (pcb_width - holeoffs*2);
				for(j=[0:1]) {
					y = holeoffs + j * (pcb_height - holeoffs*2);
					translate([x, y, 0.25])	// tolerance for pcb thickness
					cylinder(overhead, d1=stand_d * 0.9, d2=stand_d * 1.8, $fn=40);
				}
			}
		}
		
		translate([0, 0, -0.01])
		board();
		
		for(i=[0:1]) {
			x = holeoffs + i * (pcb_width - holeoffs*2);
			for(j=[0:1]) {
				y = holeoffs + j * (pcb_height - holeoffs*2);
				translate([x, y, -stand_h-pcb_thick])
				screwhole();
			}
		}
	}
}

if(have_bottom) {
	translate([0, 5, stand_h+thick+pcb_thick])
	case_bot();
}


if(have_top) {
	translate([0, -5, overhead+thick])
	rotate([180, 0, 0])
	case_top();
}