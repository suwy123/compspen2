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
(declare-const x6 Int)
(declare-const x7 Int)
(declare-const v6 Int)
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
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const v18 Int)
(declare-const x20 Int)

(assert(and
(= x18 x4)
(= x1 x2) (= x3 x4) (= x5 x6) (< x0 x20) (< x20 x7)
(< x7 x8) (< x8 x10) (< x11 x12) (< x13 x14) (< x15 x16)
(sep
(blk x0 x1)
(blk x2 x3)
(blk x4 x5)
(hls x6 x7 v6)
(pto x8 (c_data_t x9))
(blk x10 x11)
(blk x12 x13)
(blk x14 x15)
(pto x16 (c_data_t x17))
(hls x18 x19 v18)
)))

(assert(not(and
(sep
(blk x0 x20)
(blk x20 x7)
(pto x8 (c_data_t x9))
(blk x10 x11)
(blk x12 x13)
(blk x14 (+ x15 1))
(pto x16 (c_data_t x17))
))))

(check-sat)