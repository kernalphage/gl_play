@0xb6221a35dd738553;

const qux:UInt32 = 123;
struct Image {
	id @0 :UInt32;
	shortname @1 :Text; 
	timestamp @2 :UInt32;

	commands @3 :List(Command);

	struct Command {
		cmd :union{
			 type @0 :PrimitiveType;
			 state @1 :Text;
		}
		values @2:List(Float32);
		enum PrimitiveType {
			line @0;
			circle @1;
			bezier @2;
			pointCloud @3;
			polygon @4;
			# state changes
			fill @5;
			stroke @6;
		}
	}
}