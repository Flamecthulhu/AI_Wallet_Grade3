final int x = 10; //Constant
int y = 20;       

int a = 5;
double b = 3.14;
String c = "Hello";
boolean d = true;

int add(int x, int y) {
    return x + y;
}

// 呼叫
System.out.println(add(3, 4));

int n = 10;
if (n > 5) {
    System.out.println("大於 5");
} else {
    System.out.println("小於等於 5");
}

// switch
switch (n) {
    case 1:
        System.out.println("一");
        break;
    case 2:
    case 3:
        System.out.println("二或三");
        break;
    case 4: case 5: case 6: case 7: case 8: case 9: case 10:
        System.out.println("介於 4~10");
        break;
    default:
        System.out.println("其他");
}

for (int i = 1; i <= 5; i++) {
    System.out.println(i); // 1,2,3,4,5
}

for (int i = 1; i < 5; i++) {
    System.out.println(i); // 1,2,3,4
}

int m = 3;
while (m > 0) {
    System.out.println(m);
    m--;
}

class Person {
    String name;
    int age;

    Person(String name, int age) {
        this.name = name;
        this.age = age;
    }

    void sayHello() {
        System.out.println("Hi, I'm " + name + ", " + age + " years old.");
    }
}

Person p = new Person("Tom", 18);
p.sayHello();

String str = null;
if (str != null) {
    System.out.println(str.length());
} else {
    System.out.println("default");
}
