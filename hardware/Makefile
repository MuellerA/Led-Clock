OPENSCAD = /usr/bin/openscad
CP       = /bin/cp -a
RM       = /bin/rm -f

SCAD 	= Clock
STL 	= Clock-120-40-13-60-1 Clock-120-40-13-60-4 Clock-50-5-40-6-1
IMG 	= Clock1.jpg Clock2.jpg

ALL:	$(addprefix TV/,$(addsuffix .stl,$(STL))) $(addprefix TV/,$(addsuffix .scad,$(SCAD))) $(addprefix TV/,$(IMG))

Clean:
	$(RM) TV/*

file2param = $(subst -, ,$(subst TV/Clock-,,$(subst .$(2),,$(1))))

paramD = -D radius_outer=$(word 1,$(1)) -D radius_inner=$(word 2,$(1)) -D height=$(word 3,$(1)) -D ws_count=$(word 4,$(1)) -D parts=$(word 5,$(1))

TV/Clock-%.stl: Clock.scad
	$(OPENSCAD) -o $@ $(call paramD,$(call file2param,$@,stl)) $<

TV/Clock.scad: Clock.scad
	$(CP) $< $@

TV/%.jpg: images/%.jpg
	$(CP) $< $@
