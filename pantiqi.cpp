#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<map>
#include<limits.h>
#include<deque>
using namespace std;
class Flow
{
public:
	int id;
	int speed;
	int begintime;
	int sendtime;
	int needtime;
	bool issend;
	Flow(int i, int s, int b, int n);
};
class Port
{
public:
	int id;
	int speed;
	int maxspeed;
	multimap<int, Flow> flowqueue;
	deque<Flow> waitqueue;
	Port(int i, int s);
};
class Result
{
public:
	int flowid;
	int portid;
	int sendtime;
	Result(int f, int p, int s);
};
Flow::Flow(int i, int s, int b, int n)
{
	id = i;
	speed = s;
	begintime = b;
	needtime = n;
	issend = false;
	sendtime = -1;
}
Port::Port(int i, int s)
{
	id = i;
	speed = s;
	maxspeed = speed;
}
Result::Result(int f, int p, int s)
{
	flowid = f;
	portid = p;
	sendtime = s;
}

/*�������ݵ����벿�֣��������ļ�������ݶ��봦��*/
bool Input(string path, vector<Flow>& flows, vector<Port>& ports, vector<Result>& results)
{
	ifstream input;
	string path1 = path + "/flow.txt";
	string path2 = path + "/port.txt";
	string path3 = path + "/result.txt";
	input.open(path1, ios::in);
	if (!input.is_open())
		return false;
	input.ignore(100, '\n');
	/*����flow*/
	while (!input.eof())
	{
		Flow flow(-1, 0, 0, 0);
		char t;
		input >> flow.id >> t >> flow.speed >> t >> flow.begintime >> t >> flow.needtime;
		if (flow.id == -1)
			break;
		flows.push_back(flow);
	}
	input.close();
	/*flow�������*/
	input.open(path2, ios::in);
	if (!input.is_open())
		return false;
	input.ignore(100, '\n');
	/*����port*/
	while (!input.eof())
	{
		Port port(-1, 0);
		char t;
		input >> port.id >> t >> port.maxspeed;
		port.speed = port.maxspeed;
		if (port.id == -1)
			break;
		ports.push_back(port);
	}
	input.close();
	/*port�������*/
	input.open(path3, ios::in);
	if (!input.is_open())
	{
		cout << "�Ҳ�������ļ�" << endl;
		return false;
	}

	while (!input.eof())
	{
		Result res(-1,-1,-1);
		char t;
		input >> res.flowid >> t >> res.portid >> t >> res.sendtime;
		if (res.sendtime == -1)
			break;
		results.push_back(res);
	}

	return true;
}
/*���¶˿�״̬*/
int updateport(vector<Port>& ports)
{
	int time = 0;
	while (true)//ѭ��ֱ�����ж˿ڶ�û�д����͵���
	{
		int waitqueueemptycount = 0;
		for (auto& port : ports)//��ÿ���˿ڽ��д���
		{
			if (port.waitqueue.empty())//����˿��Ѿ�û�д����͵�����
				++waitqueueemptycount;
			
			for (auto j = port.flowqueue.begin(); j != port.flowqueue.end();)//�Զ˿����ѷ��͵�������Ƿ������
			{
				if (j->first > time)//mapΪ���� ������Сֵ���ڵ�ǰʱ��ʹ���û�з�����ϵ�����
				{
					break;
				}
				else//��֮���з�����ϵ��� ����ռ�õĶ˿��ڳ���
				{
					port.speed += j->second.speed;//���˿ڴ���ԭ��ȥ
					port.flowqueue.erase(j);//���ڻ�������ɾ��
					j = port.flowqueue.begin();
				}
			}
			while (!port.waitqueue.empty() && port.waitqueue.front().sendtime <= time)//��ÿ���˿��еȴ����з���ʱ��С�ڵ��ڵ�ǰʱ��������һ���Ƿ��ܷ���
			{
				if (port.waitqueue.front().speed <= port.speed)//�˿�ʣ��ռ��㹻�����Է���
				{
					port.flowqueue.insert(pair<int, Flow>(time + port.waitqueue.front().needtime, port.waitqueue.front()));//������������ѷ��Ͷ���
					port.speed -= port.waitqueue.front().speed;//���˿ڿ��ÿռ��ȥ����Ҫռ�õĿռ�
					port.waitqueue.pop_front();//���ȴ�����
				}
				else
				{
					break;
				}
			}
		}
		++time;
		if (waitqueueemptycount == ports.size())
			break;
	}
	int maxtime = time;
	for (auto& port : ports)//�������ж˿��ѷ��͵Ķ��У��ҵ���������ϵ�ʱ�䲢����
	{
		if (port.flowqueue.empty())
			continue;
		else
		{
			auto last = port.flowqueue.end();
			--last;
			maxtime = max(maxtime, last->first);
		}
	}
	return maxtime;
}
/*���ݴ���*/
int algorithm(vector<Flow>& flows, vector<Port>& ports, vector<Result>& res)
{
	if (res.size() < flows.size())
	{
		cout << "����ȱʧ�������������ʽ����" << endl;
		return 0;
	}
	for(const auto& iter:res)
	{
		int t = iter.sendtime;
		if (iter.flowid >= flows.size() || iter.flowid < 0)
		{
			cout << "��id�����ڣ�������Ϊ" << t << ',' << iter.flowid << ',' << iter.portid << endl;
			return 0;
		}
		if (iter.portid >= ports.size() || iter.portid < 0)
		{
			cout << "�˿�id�����ڣ�������Ϊ" << t << ',' << iter.flowid << ',' << iter.portid << endl;
			return 0;
		}

		Flow &flow = flows[iter.flowid];
		Port &port = ports[iter.portid];
		if (t < flow.begintime)
		{
			cout << "������ʱ��С�ڽ����豸ʱ�䣬������Ϊ" << t << ',' << iter.flowid << ',' << iter.portid << endl;
			return 0;
		}
		if (flow.speed > port.maxspeed)
		{
			cout << "��������ڶ˿�������������Ϊ" << t << ',' << iter.flowid << ',' << iter.portid << endl;
			return 0;
		}
		if (flow.issend)
		{
			cout << "�����ظ����ͣ�������Ϊ" << t << ',' << iter.flowid << ',' << iter.portid << endl;
			return 0;
		}
		/*___________________________________________________________________________________________________*/
		flow.sendtime = t;
		port.waitqueue.push_back(flow);
		flow.issend = true;
	}
	return updateport(ports);
}
double best(vector<Flow>& flows, vector<Port>& ports)
{
	long long int needspeed = 0;
	long long int cansendspeed = 0;
	for (int i = 0; i < flows.size(); ++i)
	{
		needspeed += long long int(flows[i].speed) * flows[i].needtime;
	}
	for (int i = 0; i < ports.size(); ++i)
	{
		cansendspeed += ports[i].maxspeed;
	}
	return needspeed / double(cansendspeed);
}
int main()
{
	int No = 0;
	vector<Flow> flows;
	vector<Port> ports;
	vector<Result> res;
	int alltime = 0;
	double allbest = 0;
	string path;
	while (true)
	{
		path = "../data/" + to_string(No);
		if (!Input(path, flows, ports,res))
			break;
		int thistime= algorithm(flows, ports, res);
		double thisbest = best(flows, ports);
		alltime += thistime;
		allbest += thisbest;
		cout <<"�������ţ�" << thisbest << endl;
		cout <<"ʵ�ʽ����" << thistime << endl;
		++No;
		flows.clear();
		ports.clear();
		res.clear();
	}
	cout << "�ܺ��������ţ�" << allbest << endl;
	cout << "�ܺ�ʵ�ʽ����" << alltime;
	return 0;
}
