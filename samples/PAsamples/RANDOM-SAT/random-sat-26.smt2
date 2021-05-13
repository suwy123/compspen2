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
(declare-const x2 Int)
(declare-const x3 Int)
(declare-const x4 Int)
(declare-const x5 Int)
(declare-const v4 Int)
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const v6 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const v10 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const v12 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const v14 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const v18 Int)
(declare-const x20 Int)
(declare-const x21 Int)
(declare-const x22 Int)
(declare-const x23 Int)
(declare-const v22 Int)
(declare-const x24 Int)
(declare-const x25 Int)
(declare-const v24 Int)
(declare-const x26 Int)
(declare-const x27 Int)
(declare-const v26 Int)
(declare-const x28 Int)
(declare-const x29 Int)
(declare-const x30 Int)
(declare-const x31 Int)
(declare-const x32 Int)
(declare-const x33 Int)
(declare-const x34 Int)
(declare-const x35 Int)
(declare-const v34 Int)
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
(declare-const v44 Int)
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
(declare-const v54 Int)
(declare-const x56 Int)
(declare-const x57 Int)
(declare-const x58 Int)
(declare-const x59 Int)
(declare-const v58 Int)
(declare-const x60 Int)
(declare-const x61 Int)
(declare-const x62 Int)
(declare-const x63 Int)
(declare-const v62 Int)
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
(declare-const v70 Int)
(declare-const x72 Int)
(declare-const x73 Int)
(declare-const x74 Int)
(declare-const x75 Int)
(declare-const v74 Int)
(declare-const x76 Int)
(declare-const x77 Int)
(declare-const x78 Int)
(declare-const x79 Int)
(declare-const x80 Int)
(declare-const x81 Int)
(declare-const x82 Int)
(declare-const x83 Int)
(declare-const x84 Int)
(declare-const x85 Int)
(declare-const x86 Int)
(declare-const x87 Int)
(declare-const x88 Int)
(declare-const x89 Int)
(declare-const v88 Int)
(declare-const x90 Int)
(declare-const x91 Int)
(declare-const v90 Int)
(declare-const x92 Int)
(declare-const x93 Int)
(declare-const x94 Int)
(declare-const x95 Int)
(declare-const v94 Int)
(declare-const x96 Int)
(declare-const x97 Int)
(declare-const x98 Int)
(declare-const x99 Int)
(declare-const x100 Int)
(declare-const x101 Int)
(declare-const x102 Int)
(declare-const x103 Int)
(declare-const x104 Int)
(declare-const x105 Int)
(declare-const x106 Int)
(declare-const x107 Int)
(declare-const v106 Int)
(declare-const x108 Int)
(declare-const x109 Int)
(declare-const v108 Int)
(declare-const x110 Int)
(declare-const x111 Int)
(declare-const x112 Int)
(declare-const x113 Int)
(declare-const x114 Int)
(declare-const x115 Int)
(declare-const v114 Int)
(declare-const x116 Int)
(declare-const x117 Int)
(declare-const v116 Int)
(declare-const x118 Int)
(declare-const x119 Int)

(assert(and
(distinct x59 x38) (= x15 x38) (< x7 x38) (= x91 x46) (< x6 x92) 
(sep
(pto x0 (c_data_t x1))
(pto x2 (c_data_t x3))
(hls x4 x5 v4)
(hls x6 x7 v6)
(pto x8 (c_data_t x9))
(hls x10 x11 v10)
(hls x12 x13 v12)
(hls x14 x15 v14)
(pto x16 (c_data_t x17))
(hls x18 x19 v18)
(blk x20 x21)
(hls x22 x23 v22)
(hls x24 x25 v24)
(hls x26 x27 v26)
(pto x28 (c_data_t x29))
(blk x30 x31)
(pto x32 (c_data_t x33))
(hls x34 x35 v34)
(pto x36 (c_data_t x37))
(pto x38 (c_data_t x39))
(pto x40 (c_data_t x41))
(blk x42 x43)
(hls x44 x45 v44)
(blk x46 x47)
(pto x48 (c_data_t x49))
(hls x50 x51 v50)
(blk x52 x53)
(hls x54 x55 v54)
(pto x56 (c_data_t x57))
(hls x58 x59 v58)
(blk x60 x61)
(hls x62 x63 v62)
(blk x64 x65)
(hls x66 x67 v66)
(hls x68 x69 v68)
(hls x70 x71 v70)
(blk x72 x73)
(hls x74 x75 v74)
(blk x76 x77)
(blk x78 x79)
(blk x80 x81)
(blk x82 x83)
(blk x84 x85)
(pto x86 (c_data_t x87))
(hls x88 x89 v88)
(hls x90 x91 v90)
(blk x92 x93)
(hls x94 x95 v94)
(blk x96 x97)
(pto x98 (c_data_t x99))
(blk x100 x101)
(blk x102 x103)
(pto x104 (c_data_t x105))
(hls x106 x107 v106)
(hls x108 x109 v108)
(pto x110 (c_data_t x111))
(pto x112 (c_data_t x113))
(hls x114 x115 v114)
(hls x116 x117 v116)
(blk x118 x119)
)))

(check-sat)