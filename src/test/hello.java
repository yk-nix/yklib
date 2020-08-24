public class hello {
	public native void    print_hello_world();
	public native int     yk_get_max(int i1, int i2);
	public native String  yk_add_prefix(String s);
	static {
		System.loadLibrary("hello");
	}
	public static void main(String[] args) {
		hello obj = new hello();
		obj.print_hello_world();
		System.out.printf("%d\n", obj.yk_get_max(5, 3));
		System.out.printf("%s\n", obj.yk_add_prefix("test"));
	}
}
