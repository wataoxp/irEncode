/*
 * state.h
 *
 *  Created on: Apr 24, 2026
 *      Author: watapoxp
 */

#ifndef INC_STATE_H_
#define INC_STATE_H_

#include "clockview.h"

class ISystemState{
public:
	virtual ~ISystemState() {};
	virtual ISystemState* OnValueChange(ClockView& ckv) = 0;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) = 0;
};

class StateIdle : public ISystemState{
public:
	~StateIdle() {};
	static StateIdle& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override { return this; }
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};

class StateTimeHours : public ISystemState{
public:
    ~StateTimeHours() {};
    static StateTimeHours& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};

class StateTimeMinutes : public ISystemState{
public:
    ~StateTimeMinutes() {};
    static StateTimeMinutes& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};

class StateAlmaHours : public ISystemState{
public:
    ~StateAlmaHours() {};
    static StateAlmaHours& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};

class StateAlmaMinutes : public ISystemState{
public:
    ~StateAlmaMinutes() {};
    static StateAlmaMinutes& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};

class StateAlmbHours : public ISystemState{
public:
    ~StateAlmbHours() {};
    static StateAlmbHours& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};

class StateAlmbMinutes : public ISystemState{
public:
    ~StateAlmbMinutes() {};
    static StateAlmbMinutes& Instance();
	virtual ISystemState* OnValueChange(ClockView& ckv) override;
	virtual ISystemState* OnCommand(uint32_t cmd,ClockView& ckv) override;
};


#endif /* INC_STATE_H_ */
