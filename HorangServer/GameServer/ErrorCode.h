#pragma once

enum class ErrorCode : int32
{
	SIGNIN_FAIL = 1001,	// �α��� ����


	SIGNUP_FAIL = 1004, // ȸ������ ����
	SIGNUP_ID_DUPLICATE = 1002, // ID �ߺ�
	SIGNUP_NICKNAME_DUPLICATE = 1003, // ID �ߺ�
	
	ROOM_ENTER_FAIL = 1005, // �� ���� ����
	ROOM_LEAVE_FAIL = 1006, // �� ���� ����
	ROOM_FULL = 1007, // ���� �� ��
	ROOM_NOT_FOUND = 1008, // ���� ã�� �� ����
	ROOM_ALREADY_ENTERED = 1009, // �̹� �濡 ������
	ROOM_NOT_ENTERED = 1010, // �濡 �������� ����
	ROOM_PLAYING = 1011, // ���� �÷��� ����
	ROOM_NOT_PLAYING = 1012, // ���� �÷��� ������ ����
};