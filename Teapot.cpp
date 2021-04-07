#include "pch.h"
#include "Teapot.h"
#include "Patch.h"

void Teapot::ReadVertex(void)//������Ƶ�����
{
	CStdioFile file;
	if (!file.Open(L"res\\Vertices.txt", CFile::modeRead))
	{
		MessageBox(NULL, L"can not open file!", L"Warning", MB_ICONEXCLAMATION);
		return;
	}
	CString  strLine;//�������ַ���
	int index = 0;//������������
	CString data1, data2, data3;//�������
	for (int i = 0; i < 306; i++)
	{
		file.ReadString(strLine);//���ж���
		for (int j = 0; j < strLine.GetLength(); j++)
		{
			if (' ' == strLine[j])
				index++;
			switch (index)
			{
			case 0:
				data1 += strLine[j];
				break;
			case 1:
				data2 += strLine[j];
				break;
			case 2:
				data3 += strLine[j];
				break;
			}
		}
		Ver[i].x = _wtof(data1.GetBuffer());//��������ʾ��xֵ
		Ver[i].y = _wtof(data3.GetBuffer());//��������ʾ��zֵ
		Ver[i].z = _wtof(data2.GetBuffer());//��������ʾ��yֵ
		strLine = "", data1 = "", data2 = "", data3 = "", index = 0;
	}
	file.Close();
}


void Teapot::ReadPatch(void)//����������Ƭ
{
	CStdioFile file;
	if (!file.Open(L"res\\Patches.txt", CFile::modeRead))
	{
		MessageBox(NULL, L"can not open file!", L"Warning", MB_ICONEXCLAMATION);
		return;
	}
	CString strLine;
	int index = 0;
	CString str[16];//��ſ��Ƶ�������
	for (int nPatch = 0; nPatch < 32; nPatch++)
	{
		file.ReadString(strLine);
		for (int i = 0; i < strLine.GetLength(); i++)
		{
			if (' ' == strLine[i])
				index++;
			switch (index)
			{
			case 0:
				str[0] += strLine[i];
				break;
			case 1:
				str[1] += strLine[i];
				break;
			case 2:
				str[2] += strLine[i];
				break;
			case 3:
				str[3] += strLine[i];
				break;
			case 4:
				str[4] += strLine[i];
				break;
			case 5:
				str[5] += strLine[i];
				break;
			case 6:
				str[6] += strLine[i];
				break;
			case 7:
				str[7] += strLine[i];
				break;
			case 8:
				str[8] += strLine[i];
				break;
			case 9:
				str[9] += strLine[i];
				break;
			case 10:
				str[10] += strLine[i];
				break;
			case 11:
				str[11] += strLine[i];
				break;
			case 12:
				str[12] += strLine[i];
				break;
			case 13:
				str[13] += strLine[i];
				break;
			case 14:
				str[14] += strLine[i];
				break;
			case 15:
				str[15] += strLine[i];
				break;
			}
		}
		for (int nVertex = 0; nVertex < 16; nVertex++)
			Pat[nPatch][nVertex] = (_wtoi(str[nVertex].GetBuffer()));//������ʾ�Ŀ��Ƶ�������
		strLine = "";
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			str[nVertex] = "";
		}
		index = 0;
	}
	file.Close();
}

void Teapot::Init()//��������
{
	bezier.DeleteObjects();
	bezier.SetColor(reflectance);
	bezier.SetEmmision(emmision);
	bezier.SetRecursionTimes(divide);
	bezier.Accelerate(); // ע�����Ĭ��ʹ�ð�Χ�м��٣�����Ҫ����ע�͵�����
	//������Ƭ���˳�򻮷ֲ���
	DrawRim();//����
	DrawBody();//����
	DrawHandle();//����
	DrawSpout();//����
	DrawLid();//����	
	DrawBottom();//����
	this->box = bezier.GetBoundingBox();
}

void Teapot::DrawRim()//���ƺ���
{
	for (int nPatch = 0; nPatch < 4; nPatch++)
	{
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			Ver3[nVertex] = Ver[Pat[nPatch][nVertex] - 1];
		}
		SigleCurvedPatch();//˫����Bezier����
	}
}


void Teapot::DrawBody()//���ƺ���
{
	for (int nPatch = 4; nPatch < 12; nPatch++)
	{
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			Ver3[nVertex] = Ver[Pat[nPatch][nVertex] - 1];
		}
		SigleCurvedPatch();//˫����Bezier����
	}
}


void Teapot::DrawBottom()//���ƺ���
{
	for (int nPatch = 28; nPatch < 32; nPatch++)
	{
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			Ver3[nVertex] = Ver[Pat[nPatch][nVertex] - 1];
		}
		SigleCurvedPatch();//˫����Bezier����
	}
}

void Teapot::DrawHandle()//���ƺ���
{
	for (int nPatch = 12; nPatch < 16; nPatch++)
	{
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			Ver3[nVertex] = Ver[Pat[nPatch][nVertex] - 1];
		}
		SigleCurvedPatch();//˫����Bezier����
	}
}

void Teapot::DrawSpout()//���ƺ���
{
	for (int nPatch = 16; nPatch < 20; nPatch++)
	{
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			Ver3[nVertex] = Ver[Pat[nPatch][nVertex] - 1];
		}
		SigleCurvedPatch();//˫����Bezier����
	}
}

void Teapot::DrawLid()//���ƺ���
{
	for (int nPatch = 20; nPatch < 28; nPatch++)
	{
		for (int nVertex = 0; nVertex < 16; nVertex++)
		{
			Ver3[nVertex] = Ver[Pat[nPatch][nVertex] - 1];
		}
		SigleCurvedPatch();//˫����Bezier����
	}
}