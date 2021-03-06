//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chugerate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"

// general includes
#include <stdio.h>
#include <limits.h>

// ibniz include
#include "ibniz.h"

// declaration of chugin constructor
CK_DLL_CTOR(ibniz_ctor);
// declaration of chugin desctructor
CK_DLL_DTOR(ibniz_dtor);

// example of getter/setter
CK_DLL_MFUN(ibniz_setParam);
CK_DLL_MFUN(ibniz_getParam);
CK_DLL_MFUN(ibniz_setCode);
CK_DLL_MFUN(ibniz_getCode);
CK_DLL_MFUN(ibniz_on);
CK_DLL_MFUN(ibniz_off);
CK_DLL_MFUN(ibniz_reset);

// for Chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICK(ibniz_tick);

// this is a special offset reserved for Chugin internal data
t_CKINT ibniz_data_offset = 0;


// class definition for internal Chugin data
// (note: this isn't strictly necessary, but serves as example
// of one recommended approach)
class ibniz
{
public:


    // constructor
    ibniz( t_CKFLOAT fs)
    {
    	active = 1;
    	srate_f = fs;
        m_param = 0;
	vm_p = new vm_t;

    	remainder_d = 0;
 	last_freq =0;
    	ib_sample_per_tick = 1;
    	last_ib_out = 0;
	
	// init the ibniz virtual machine with an empty string
	vm_init(vm_p);
        vm_compile((char *)"", vm_p);

    }
    
    // destructor	 
    ~ibniz()
    {
    	delete vm_p;
    }


    // for Chugins extending UGen
    SAMPLE tick( SAMPLE in )
    {
  	int32_t ib_in;
  	int32_t ib_out;
	SAMPLE out;
	int nb_ibniz_sample;

	// Convert input to ibniz data format => To be retried later to use ibniz as a filter/effect.
	//ib_in = (int32_t)((in + 1)*0x8000);

	if (active) {
	nb_ibniz_sample = compute_nb_of_ibniz_run (in);

	if (nb_ibniz_sample == 0){
	 	ib_out = last_ib_out;
	}
	else {
		for (int i=0; i<nb_ibniz_sample; i++) {	
			ib_out = vm_run(ib_in, vm_p);
		}
	
		last_ib_out = ib_out;
	}


	out = (SAMPLE)(ib_out & 0xFFFF);	
	out = (out / 0x8000) - 1;

        return out;
	}
	else {
	    return 0;
	}
	
    }

    // set parameter example
    float setParam( t_CKFLOAT p )
    {
//        m_param = p;
	vm_p->test = p;
        return p;
    }

    // get parameter example
    float getParam() { return vm_p->test /*m_param*/; }


    void set_code(Chuck_String * in) {
    	// save code
	code_s = in; 
        
       // Copy string into a char * buffer
       size_t size = in->str.size() + 1;
       char * buffer = new char[ size ];
       // copier la cha�ne
       strncpy( buffer, in->str.c_str(), size );
       
       vm_compile(buffer, vm_p);
       
       delete buffer;
    }

    Chuck_String * get_code(void) {return code_s;}
    
    void on () {
    	active = 1;
    }

    void off () {
    	active = 0;
    }

    void reset() {
	vm_init(vm_p);
	set_code(code_s);
    }

private:
    // instance data
    int active;
    
    float m_param;
 
    float srate_f;
   
    Chuck_String * code_s;

    // ibniz virtual machine context
    vm_t * vm_p;
   
    // Parameter to manage frequency conversion
    double remainder_d ;
    double last_freq ;
    double  ib_sample_per_tick ;

    int32_t last_ib_out ;

    int compute_nb_of_ibniz_run (double freq) {
	
	int rounded_sample_per_tick;
	
	if (freq == 0.0 /* Assuming no input */) freq = 60.;
//	else printf ("other freq %lf",freq);
	
		
	if (freq != last_freq){
		// RESET
	//	remainder_d = 0;
		last_freq =freq;
		ib_sample_per_tick = freq * 1024. / srate_f; // Ju: TODO : Dynamic srate
	}
	
		
	rounded_sample_per_tick = (int) (ib_sample_per_tick + remainder_d);
	remainder_d = (ib_sample_per_tick + remainder_d) - rounded_sample_per_tick;

	return rounded_sample_per_tick;

    
    }
    
    
};


// query function: chuck calls this when loading the Chugin
// NOTE: developer will need to modify this function to
// add additional functions to this Chugin
CK_DLL_QUERY( ibniz )
{
    // hmm, don't change this...
    QUERY->setname(QUERY, "ibniz");
    
    // begin the class definition
    // can change the second argument to extend a different ChucK class
    QUERY->begin_class(QUERY, "ibniz", "UGen");

    // register the constructor (probably no need to change)
    QUERY->add_ctor(QUERY, ibniz_ctor);
    // register the destructor (probably no need to change)
    QUERY->add_dtor(QUERY, ibniz_dtor);
    
    // for UGen's only: add tick function
    QUERY->add_ugen_func(QUERY, ibniz_tick, NULL, 1, 1);
    
    // NOTE: if this is to be a UGen with more than 1 channel, 
    // e.g., a multichannel UGen -- will need to use add_ugen_funcf()
    // and declare a tickf function using CK_DLL_TICKF

    // example of adding setter method
    QUERY->add_mfun(QUERY, ibniz_setParam, "float", "param");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "float", "arg");

    // example of adding getter method
    QUERY->add_mfun(QUERY, ibniz_getParam, "float", "param");
    
    QUERY->add_mfun(QUERY, ibniz_setCode, "string", "code");
    // example of adding argument to the above method
    QUERY->add_arg(QUERY, "string", "arg");

    // example of adding getter method
    QUERY->add_mfun(QUERY, ibniz_getParam, "float", "param");
    
    QUERY->add_mfun(QUERY, ibniz_getCode, "string", "code");
    // this reserves a variable in the ChucK internal class to store 
    // referene to the c++ class we defined above
    ibniz_data_offset = QUERY->add_mvar(QUERY, "int", "@i_data", false);

   QUERY->add_mfun(QUERY, ibniz_on, "void", "on");
   QUERY->add_mfun(QUERY, ibniz_off, "void", "off");
   QUERY->add_mfun(QUERY, ibniz_reset, "void", "reset");



    // end the class definition
    // IMPORTANT: this MUST be called!
    QUERY->end_class(QUERY);

    // wasn't that a breeze?
    return TRUE;
}


// implementation for the constructor
CK_DLL_CTOR(ibniz_ctor)
{
    // get the offset where we'll store our internal c++ class pointer
    OBJ_MEMBER_INT(SELF, ibniz_data_offset) = 0;
    
    // instantiate our internal c++ class representation
    ibniz * bcdata = new ibniz(API->vm->get_srate());

    // store the pointer in the ChucK object member
    OBJ_MEMBER_INT(SELF, ibniz_data_offset) = (t_CKINT) bcdata;
}


// implementation for the destructor
CK_DLL_DTOR(ibniz_dtor)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // check it
    if( bcdata )
    {
        // clean up
        delete bcdata;
        OBJ_MEMBER_INT(SELF, ibniz_data_offset) = 0;
        bcdata = NULL;
    }
}


// implementation for tick function
CK_DLL_TICK(ibniz_tick)
{
    // get our c++ class pointer
    ibniz * c = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
 
    // invoke our tick function; store in the magical out variable
    if(c) *out = c->tick(in);

    // yes
    return TRUE;
}


// example implementation for setter
CK_DLL_MFUN(ibniz_setParam)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    RETURN->v_float = bcdata->setParam(GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(ibniz_setCode)
{
    Chuck_String * code_s;    
    
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    code_s= GET_NEXT_STRING(ARGS);    



    bcdata->set_code(code_s);    
   // vm_compile(buffer, &(bcdata->vm));
    
    RETURN->v_float = 0;
}


// example implementation for getter
// example implementation for getter
CK_DLL_MFUN(ibniz_getParam)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    RETURN->v_float = bcdata->getParam();
}

CK_DLL_MFUN(ibniz_getCode)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    RETURN->v_string = bcdata->get_code();
}

CK_DLL_MFUN(ibniz_on)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    bcdata->on();
}

CK_DLL_MFUN(ibniz_off)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    bcdata->off();
}

CK_DLL_MFUN(ibniz_reset)
{
    // get our c++ class pointer
    ibniz * bcdata = (ibniz *) OBJ_MEMBER_INT(SELF, ibniz_data_offset);
    // set the return value
    bcdata->reset();
}

