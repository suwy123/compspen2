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
(declare-const x8 Int)
(declare-const x9 Int)

(assert(and
(= x3 x7) (< x7 x1) (distinct x7 x5) (= x3 x0) (= x1 x4)
(sep
(blk x0 x1)
(blk x2 x3)
(blk x4 x5)
(pto x6 (c_data_t x7))
(blk x8 x9)
)))

(assert(not(and
(= x3 x7) (< x7 x1) (distinct x7 x5) 
(sep
(blk x2 x5)
(pto x6 (c_data_t x7))
(blk x8 x9)
))))

(check-sat)