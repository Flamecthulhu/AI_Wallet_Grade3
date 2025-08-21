import Foundation

let myAge = 15
let yourAge = 17

if myAge > yourAge {
    "yee"
}
else if yourAge > myAge {
    "Not yee"
}
else {
    "wthef"
}

//Binary infix
let yourmumsAge = myAge + 5
let doublemyAge = myAge * 2

//Unary prefix
var foo = !true
let sheit = "foo" + " " + "shiet"
//Unary postfix
let name = Optional("Allan")
type(of: name)
let unaryPostfix = name!
type(of: unaryPostfix)

let age = 15
//let message: String
//if age >= 18{
//    message = "Mmm"
//}
//else{
//    message = "sus"
//}
let message = age >= 18 ? "Mmm" : "sus"
