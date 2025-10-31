#ifndef _ITFCLIENTUSER_PERFORCE_H_
#define _ITFCLIENTUSER_PERFORCE_H_


namespace  ITF
{
	class ITFClientUser : public ClientUser
	{
		virtual void 	OutputError( const char *errBuf );
		virtual void	OutputInfo( char level, const char *data );
		virtual void 	OutputBinary( const char *data, int length );
		virtual void 	OutputText( const char *data, int length );

		virtual void	OutputStat( StrDict *varList );

	};    


} // namespace ITF

#endif //_ADAPTERSOURCECONTROL_PERFORCE_H_