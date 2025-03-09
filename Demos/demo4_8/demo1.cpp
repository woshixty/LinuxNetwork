// 演示循环队列的使用
#include "_public.h"

int main()
{
	using ElemType=int;
	squeue<ElemType, 5> QQ;
	QQ.init();
	ElemType ee;	// 创建一个数据元素
	
	cout << "元素 (1 2 3) 入队\n";
	ee=1; QQ.push(ee);
	ee=2; QQ.push(ee);
	ee=3; QQ.push(ee);
	cout << "队列长度是" << QQ.size() << endl;
	QQ.printqueue();
	
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	QQ.printqueue();

	ee=4; QQ.push(ee);
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	
	ee=5; QQ.push(ee);
	ee=6; QQ.push(ee);
	ee=7; QQ.push(ee);
	ee=8; QQ.push(ee);
	ee=9; QQ.push(ee);
	ee=10; QQ.push(ee);
	QQ.printqueue();

	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	ee=QQ.front(); QQ.pop(); cout << "出队的元素值为" << ee << endl;
	return 0;
}
