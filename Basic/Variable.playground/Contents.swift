import Foundation

let fruit = "apple"
let cartlist = "banana"

var wishlist = [
    fruit,
    cartlist]

wishlist = ["1", "2"]
wishlist.append("yourmum")

let foo = "Foo"
var foo2 = foo
foo2 = "foo"

var moresheit = ["sheit", "moresheit"]
let copysheit = moresheit

moresheit.append("sheit")

let oldArray = NSMutableArray(
    array: [
        "apple",
        "banana"
    ]
)

oldArray.add(9)
var newArray = oldArray
newArray.add("none")
oldArray
newArray


let someSheits = NSMutableArray(
    array: [
        "apple",
        "banana"
    ]
)

func changingSheit (_ array: NSArray) {
    let copy = array as! NSMutableArray //DONT WRITING LIKE THIS
    copy.add("sa")
}

changingSheit(someSheits)
someSheits
