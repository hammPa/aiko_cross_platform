# Aiko Programming Language

**Aiko** adalah bahasa pemrograman berbasis C++ dan LLVM-14. Bahasa ini mendukung tipe data statis dan dinamis dengan sintaks sederhana, cocok untuk eksperimen belajar compiler dan interpreter.

## Fitur Utama

- **Tipe Data**: i32, i64, f32, f64, bool, str, struct, array (1D & 2D)  
- **Variabel**: deklarasi, assignment, shadowing, operasi +=, -=, *=, /=, %=  
- **Operator**: Unary (-, !), Binary (+, -, *, /, %)  
- **Kontrol Alur**: for, if, elif, else, break, continue  
- **Fungsi**: procedure dan fungsi dengan return, parameter eksplisit, dan shadowing  
- **Input & Output**: print, input dengan tipe data spesifik  
- **Struct**: deklarasi, inisialisasi, member access, tipe data eksplisit  
- **Array**: akses, assignment, operasi pada index, mendukung array multi-dimensi  
- **Tipe Data Dinamis & Statik**: variabel bisa statik atau dinamis, dengan pengecekan tipe  
- **Fitur Tambahan**: typeof untuk mendeteksi tipe data variabel, kalkulator sederhana dengan input  

## Contoh Penggunaan

```cpp
// Variabel sederhana
var x = 10;
print(x);

// Array
var arr = [1, 2, 3];
print(arr);
arr[1] = 99;
print(arr);

// Loop dan scope
for i = 0 .. 5 {
    if(i == 2) continue;
    if(i == 4) break;
    print(i);
}

// If-Elif-Else
var y = 10;
if(y <= 4) {
    print("y <= 4");
}
elif(y <= 8) {
    print("y <= 8");
}
else {
    print("y > 8");
}

// Unary & Binary Operator
var a: i32 = -10;
var b = 5 + 3 * 2;
print(a);
print(b);

// Fungsi dengan return
fun add(x: i32, y: i32){
    return x + y;
}
var result = add(10, 20);
print(result);

// Fungsi procedure
fun displayX(x){
    print(x);
}
displayX(5);

// Shadowing
var outerVar = 10;
fun display(){
    var outerVar = 20;
    print(outerVar);
}
display();
print(outerVar);

// Fungsi dengan parameter eksplisit
var outerVarInt: i32 = 10;
fun displayPassingInt(outerVarInt: i32){
    print(outerVarInt);
}
displayPassingInt(outerVarInt);
print(outerVarInt);

// Struct
struct Person {
    name: str,
    age: i32
}
var person = Person{name: "Alice", age: 30};
print(person.name);
print(person.age);

// Input
var name = input("Enter your name: ", "string");
print(name);

// Typeof
var intVar = 10;
print(typeof(intVar));

// Array 2D (belum sepenuhnya stabil)
var matrix = [[1,2],[3,4]];
print(matrix);
```

# Keterangan

Bahasa ini masih dalam tahap pengembangan; beberapa fitur seperti return string dan array 2D belum sepenuhnya stabil.
Dirancang untuk pembelajaran compiler, interpretasi, dan eksperimen dengan LLVM.