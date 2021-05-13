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
(declare-const x16 Int)
(declare-const x17 Int)
(declare-const x18 Int)
(declare-const x19 Int)
(declare-const x20 Int)
(declare-const x21 Int)

(assert(and
(distinct x8 x13) (distinct x9 x8) (= x1 x2) (= x3 x4) (= x5 x6) (= x7 x8)
(= (+ x8 1) x10) (= x11 x12) (= x13 x14) (< x4 x20) (< x20 x5) (< x6 x21) (< x21 x7)
(sep
(blk x0 x1)
(blk x2 x3)
(hls x4 x5 v4)
(hls x6 x7 v6)
(pto x8 (c_data_t x9))
(hls x10 x11 v10)
(hls x12 x13 v12)
(blk x14 x15)
(pto x16 (c_data_t x17))
(pto x18 (c_data_t x19))
)))

(assert(not(and
(distinct x8 x13) (distinct x9 x8) 
(sep
(blk x0 x20)
(blk x20 x21)
(blk x21 x15)
(pto x16 (c_data_t x17))
(pto x18 (c_data_t x19))
))))

(check-sat)