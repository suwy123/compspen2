(set-logic QF_SLAH)

(declare-datatypes ((data_t 0)) (((c_data_t (sz Int)))))

(declare-heap (Int data_t))

(define-fun hck ((?x Int) (?y Int) (?v Int)) Bool
	(and
		(>= (- ?y ?x) 2) (<= (- ?y ?x) ?v)
		(sep (pto ?x (c_data_t (- ?y ?x))) (blk (+ ?x 1) ?y))
))

(define-fun-rec hls ((?x Int) (?y Int) (?v Int)) Bool
	(or
		(and (= ?x ?y) (_ emp Int data_t))
		(exists ((?w Int)) (sep (hck ?x ?w ?v) (hls ?w ?y ?v)))
))

(declare-const x0 Int)
(declare-const x1 Int)
(declare-const v0 Int)
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const v4 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const v16 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)
(declare-const x22 Int)
(declare-const x23 Int)
(declare-const x24 Int)
(declare-const x25 Int)
(declare-const x26 Int)
(declare-const x27 Int)
(declare-const x28 Int)
(declare-const x29 Int)
(declare-const v28 Int)
(declare-const x30 Int)
(declare-const x31 Int)
(declare-const x32 Int)
(declare-const x33 Int)
(declare-const x34 Int)
(declare-const x35 Int)
(declare-const x36 Int)
(declare-const x37 Int)
(declare-const x38 Int)
(declare-const x39 Int)
(declare-const x40 Int)
(declare-const x41 Int)
(declare-const x42 Int)
(declare-const x43 Int)
(declare-const x44 Int)
(declare-const x45 Int)
(declare-const x46 Int)
(declare-const x47 Int)
(declare-const x48 Int)
(declare-const x49 Int)
(declare-const x50 Int)
(declare-const x51 Int)
(declare-const v50 Int)
(declare-const x52 Int)
(declare-const x53 Int)
(declare-const x54 Int)
(declare-const x55 Int)
(declare-const x56 Int)
(declare-const x57 Int)
(declare-const x58 Int)
(declare-const x59 Int)
(declare-const x60 Int)
(declare-const x61 Int)
(declare-const x62 Int)
(declare-const x63 Int)
(declare-const x64 Int)
(declare-const x65 Int)
(declare-const x66 Int)
(declare-const x67 Int)
(declare-const v66 Int)
(declare-const x68 Int)
(declare-const x69 Int)
(declare-const v68 Int)
(declare-const x70 Int)
(declare-const x71 Int)
(declare-const x72 Int)
(declare-const x73 Int)
(declare-const x74 Int)
(declare-const x75 Int)
(declare-const x76 Int)
(declare-const x77 Int)
(declare-const v76 Int)
(declare-const x78 Int)
(declare-const x79 Int)
(declare-const x80 Int)
(declare-const x81 Int)
(declare-const x82 Int)
(declare-const x83 Int)
(declare-const v82 Int)
(declare-const x84 Int)
(declare-const x85 Int)
(declare-const v84 Int)
(declare-const x86 Int)
(declare-const x87 Int)
(declare-const v86 Int)
(declare-const x88 Int)
(declare-const x89 Int)
(declare-const v88 Int)
(declare-const x90 Int)
(declare-const x91 Int)
(declare-const v90 Int)
(declare-const x92 Int)
(declare-const x93 Int)
(declare-const v92 Int)
(declare-const x94 Int)
(declare-const x95 Int)
(declare-const x96 Int)
(declare-const x97 Int)
(declare-const x98 Int)
(declare-const x99 Int)
(declare-const v98 Int)
(declare-const x100 Int)
(declare-const x101 Int)
(declare-const v100 Int)
(declare-const x102 Int)
(declare-const x103 Int)
(declare-const x104 Int)
(declare-const x105 Int)
(declare-const x106 Int)
(declare-const x107 Int)
(declare-const x108 Int)
(declare-const x109 Int)

(assert(and
(< x25 x24) (distinct x86 x39) 
(sep
(hls x0 x1 v0)
(blk x2 x3)
(hls x4 x5 v4)
(blk x6 x7)
(pto x8 (c_data_t x9))
(blk x10 x11)
(blk x12 x13)
(blk x14 x15)
(hls x16 x17 v16)
(blk x18 x19)
(blk x20 x21)
(blk x22 x23)
(blk x24 x25)
(blk x26 x27)
(hls x28 x29 v28)
(blk x30 x31)
(pto x32 (c_data_t x33))
(pto x34 (c_data_t x35))
(blk x36 x37)
(pto x38 (c_data_t x39))
(pto x40 (c_data_t x41))
(pto x42 (c_data_t x43))
(pto x44 (c_data_t x45))
(pto x46 (c_data_t x47))
(pto x48 (c_data_t x49))
(hls x50 x51 v50)
(blk x52 x53)
(pto x54 (c_data_t x55))
(blk x56 x57)
(pto x58 (c_data_t x59))
(blk x60 x61)
(blk x62 x63)
(pto x64 (c_data_t x65))
(hls x66 x67 v66)
(hls x68 x69 v68)
(pto x70 (c_data_t x71))
(pto x72 (c_data_t x73))
(pto x74 (c_data_t x75))
(hls x76 x77 v76)
(pto x78 (c_data_t x79))
(pto x80 (c_data_t x81))
(hls x82 x83 v82)
(hls x84 x85 v84)
(hls x86 x87 v86)
(hls x88 x89 v88)
(hls x90 x91 v90)
(hls x92 x93 v92)
(blk x94 x95)
(blk x96 x97)
(hls x98 x99 v98)
(hls x100 x101 v100)
(blk x102 x103)
(blk x104 x105)
(blk x106 x107)
(blk x108 x109)
)))

(check-sat)