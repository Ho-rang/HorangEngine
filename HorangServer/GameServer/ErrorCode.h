#pragma once

enum class ErrorCode : int32
{
	SIGNIN = 1000, // �α���
	SIGNIN_FAIL,	// �α��� ����
	SIGNIN_DUPLICATE, // �ߺ� �α���
	SIGNIN_AUTOLOIGN_FAIL, // �ڵ� �α��� ����
	SIGNIN_ALREADY, // �̹� �α��� ��

	SIGNUP = 2000, // ȸ������
	SIGNUP_FAIL, // ȸ������ ����
	SIGNUP_ID_DUPLICATE, // ID �ߺ�
	SIGNUP_NICKNAME_DUPLICATE, // ID �ߺ�

	ROOM = 5000, // ��
	ROOM_ENTER_FAIL, // �� ���� ����
	ROOM_LEAVE_FAIL, // �� ���� ����
	ROOM_PASSWORD_INCORRECT, // �� ��й�ȣ Ʋ��
	ROOM_FULL, // ���� �� ��
	ROOM_NOT_FOUND, // ���� ã�� �� ����
	ROOM_ALREADY_ENTERED, // �̹� �濡 ������
	ROOM_NOT_ENTERED, // �濡 �������� ����
	ROOM_PLAYING, // ���� �÷��� ����
	ROOM_NOT_PLAYING, // ���� �÷��� ������ ����
	ROOM_NOT_HOST, // ������ �ƴ�
	ROOM_NOT_ENOUGH_PLAYER, // �ο��� ����
	ROOM_ENDING, // ���� ������
};