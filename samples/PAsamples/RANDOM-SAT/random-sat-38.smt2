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
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const v6 Int)
(declare-const x8 Int)
(declare-const x9 Int)
(declare-const v8 Int)
(declare-const x10 Int)
(declare-const x11 Int)
(declare-const x12 Int)
(declare-const x13 Int)
(declare-const v12 Int)
(declare-const x14 Int)
(declare-const x15 Int)
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const v18 Int)
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
(declare-const x30 Int)
(declare-const x31 Int)
(declare-const x32 Int)
(declare-const x33 Int)
(declare-const x34 Int)
(declare-const x35 Int)
(declare-const v34 Int)
(declare-const x36 Int)
(declare-const x37 Int)
(declare-const v36 Int)
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
(declare-const v48 Int)
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
(declare-const v62 Int)
(declare-const x64 Int)
(declare-const x65 Int)
(declare-const x66 Int)
(declare-const x67 Int)
(declare-const x68 Int)
(declare-const x69 Int)
(declare-const v68 Int)
(declare-const x70 Int)
(declare-const x71 Int)
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
(declare-const v80 Int)
(declare-const x82 Int)
(declare-const x83 Int)
(declare-const v82 Int)
(declare-const x84 Int)
(declare-const x85 Int)
(declare-const v84 Int)
(declare-const x86 Int)
(declare-const x87 Int)
(declare-const x88 Int)
(declare-const x89 Int)
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
(declare-const v96 Int)
(declare-const x98 Int)
(declare-const x99 Int)
(declare-const v98 Int)
(declare-const x100 Int)
(declare-const x101 Int)
(declare-const x102 Int)
(declare-const x103 Int)
(declare-const v102 Int)
(declare-const x104 Int)
(declare-const x105 Int)
(declare-const x106 Int)
(declare-const x107 Int)
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
(declare-const x118 Int)
(declare-const x119 Int)
(declare-const x120 Int)
(declare-const x121 Int)
(declare-const x122 Int)
(declare-const x123 Int)
(declare-const v122 Int)
(declare-const x124 Int)
(declare-const x125 Int)
(declare-const x126 Int)
(declare-const x127 Int)
(declare-const v126 Int)
(declare-const x128 Int)
(declare-const x129 Int)
(declare-const v128 Int)

(assert(and
(< x97 x62) (= x50 x15) (< x112 x80) (= x20 x98) (= x61 x53) (< x58 x55) (distinct x30 x16) (< x39 x94) (< x109 x5) (distinct x111 x129) (< x117 x77) (distinct x108 x43) (= x2 x86) (distinct x86 x120) (< x69 x69) 
(sep
(hls x0 x1 v0)
(pto x2 (c_data_t x3))
(blk x4 x5)
(hls x6 x7 v6)
(hls x8 x9 v8)
(blk x10 x11)
(hls x12 x13 v12)
(pto x14 (c_data_t x15))
(blk x16 x17)
(hls x18 x19 v18)
(pto x20 (c_data_t x21))
(blk x22 x23)
(blk x24 x25)
(blk x26 x27)
(blk x28 x29)
(pto x30 (c_data_t x31))
(pto x32 (c_data_t x33))
(hls x34 x35 v34)
(hls x36 x37 v36)
(pto x38 (c_data_t x39))
(pto x40 (c_data_t x41))
(pto x42 (c_data_t x43))
(blk x44 x45)
(pto x46 (c_data_t x47))
(hls x48 x49 v48)
(hls x50 x51 v50)
(blk x52 x53)
(blk x54 x55)
(blk x56 x57)
(pto x58 (c_data_t x59))
(blk x60 x61)
(hls x62 x63 v62)
(blk x64 x65)
(pto x66 (c_data_t x67))
(hls x68 x69 v68)
(blk x70 x71)
(pto x72 (c_data_t x73))
(hls x74 x75 v74)
(pto x76 (c_data_t x77))
(blk x78 x79)
(hls x80 x81 v80)
(hls x82 x83 v82)
(hls x84 x85 v84)
(blk x86 x87)
(blk x88 x89)
(hls x90 x91 v90)
(blk x92 x93)
(hls x94 x95 v94)
(hls x96 x97 v96)
(hls x98 x99 v98)
(pto x100 (c_data_t x101))
(hls x102 x103 v102)
(blk x104 x105)
(pto x106 (c_data_t x107))
(hls x108 x109 v108)
(blk x110 x111)
(pto x112 (c_data_t x113))
(hls x114 x115 v114)
(pto x116 (c_data_t x117))
(pto x118 (c_data_t x119))
(pto x120 (c_data_t x121))
(hls x122 x123 v122)
(pto x124 (c_data_t x125))
(hls x126 x127 v126)
(hls x128 x129 v128)
)))

(check-sat)