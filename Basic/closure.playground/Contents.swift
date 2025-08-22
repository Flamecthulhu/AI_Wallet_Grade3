import Foundation

//func add(_ val1: Int, _ val2: Int) -> Int {
//
//}

let add: (Int, Int) -> Int
   = { (lhs: Int, rhs: Int) -> Int in
       lhs + rhs
   }
add(1, 2)

func func2(_ lhs: Int, _ rhs: Int, using function: (Int, Int) -> Int) -> Int {
    function(lhs, rhs)
}
func2(2, 3) {(lhs, rhs) in lhs + rhs}
func2(2, 3) {$0 + $1}

let array = [1,5,6,2,4]
array.sorted(by: {(lhs: Int, rhs: Int) -> Bool in
    lhs < rhs
})
