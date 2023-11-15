#include "ServerConfig_class.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

ServerConfig_class::ServerConfig_class()
{
}

ServerConfig_class::ServerConfig_class( const ServerConfig_class & src )
{
}


/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

ServerConfig_class::~ServerConfig_class()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

ServerConfig_class &				ServerConfig_class::operator=( ServerConfig_class const & rhs )
{
	//if ( this != &rhs )
	//{
		//this->_value = rhs.getValue();
	//}
	return *this;
}

std::ostream &			operator<<( std::ostream & o, ServerConfig_class const & i )
{
	//o << "Value = " << i.getValue();
	return o;
}


/*
** --------------------------------- METHODS ----------------------------------
*/

/// @brief Checks wether a file exists and if the user has access to it
/// @param filePath string representing the path of the file
/// @return true or false logically
bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}

/// @brief Checks wether a string is numeric or not
/// @param str The string to be checked
/// @return true or false logically
bool isNumeric(const std::string& str) {
    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
        if (!isdigit(*it)) {
            return false;
        }
    }
    return true;
}

/// @brief This function validates and populates the server_name member of the class
/// @param it Constant iterator of othervals map
/// @return 0 on success or Err_ServerName on error
int	ServerConfig_class::servName_validate_fill(otherVals_itc it)
{
	if (it->first == "server_name") {
		if (it->second.empty())			//check if the it->second is not empty
			return Err_ServerName;		//error then I'd also have to log it somehow
		server_name = it->second;
	}
	return 0;
}

/// @brief This function validates and populates the max_body_size member of the class
/// @param it Constant iterator of othervals map
/// @return Warn_None, Err_None on success or Err_BodySize_Unit, Err_BodySize_Numval or Warn_BodySize_Missing
int	ServerConfig_class::maxBodySize_validate_fill(otherVals_itc it)
{
	if (it->first == "client_max_body_size") {
		std::string	val = it->second.at(0);							//possibly not existing
		int			numVal = std::atoi(it->second.at(0).c_str());	//maybe return error if this shit is 0
		char	c = val.back();

		if (val.empty())
			return Err_BodySize_Unit;
		if (numVal <= 0)
			return Err_BodySize_Numval;
		switch (c) {
		case K:
			numVal * K;
			break;
		case M:
			numVal * M;
			break;
		case G:
			numVal * G;
			break;
		default:
			return Err_BodySize_Unit;
		}
		max_body_size = numVal;
		return Warn_None, Err_None;
	}
	return Warn_BodySize_Missing;
}

/// @brief This function validates and populates the port member of the class
/// @param it Constant iterator of othervals map
/// @return Warn_None, Err_None on success or Err_Port_WrongParam and Warn_Port_Missing
int	ServerConfig_class::host_port_validate_fill(otherVals_itc it)
{
	if (it->first == "listen") {
		int	result = std::atoi(it->second.at(0).c_str()); //return err if this is empty I guess
		if (result > 0 && result < MaxPortNum)
			port = result;
		else
			Err_Port_WrongParam;//error
		return Warn_None, Err_None;
	}
	return Warn_Port_Missing;
}

/// @brief This function validates and populates the err_pages member of the class
/// @param it Constant iterator of othervals map
/// @return Err_None, Warn_None on success or Err_ErrPage_File and Warn_ErrPage_Missing
int	ServerConfig_class::errorPages_validate_fill(otherVals_itc it) {
	int					error;
	ErrorPage			page;

	if (it->first == "error_page") {
		page.path = it->second.back();					// check if its null
		if (page.path.empty() || !fileExists(page.path))
			return Err_ErrPage_File;					//log error

		for (std::vector<std::string>::const_iterator i = it->second.begin();
		i != it->second.end(); i++) {
			if (*i != it->second.back())
				continue;
			if (!isNumeric(*i))
				continue;								//return an error
			error = std::atoi(i->c_str());
			if (error <= 0)
				continue;								//return an error
			page.errs.push_back(error);
		}
		err_pages.push_back(page);
		return Err_None, Warn_None;
	}
	return Warn_ErrPage_Missing;
}

/// @brief Iterates through otherVals map and assigns fills in the member
///		variables of the class accordingly (also validating the integrity
///		and correctness of the data)
/// @param  
void	ServerConfig_class::mapToValues( void ) {
	//if the static bool is not set to true set it
	if (!dhp_set)
		dhp_set = true;
	for (otherVals_itc it = other_vals.begin(); it != other_vals.end(); it++) {
		//set server_name
		if (servName_validate_fill(it)) {
			//do error
		}
		//set max_bodysize
		if (maxBodySize_validate_fill(it)) {
			//do error
		}
		//set host and port
		if (host_port_validate_fill(it)) {
			//do error
		}
		//set error_pages
		if (errorPages_validate_fill(it)) {
			//do error
		}
	}
}

void	ServerConfig_class::printValues( void ) {
	std::cout << "Server name: ";
	for(auto c : server_name) {
		std::cout << c << " ";
	} std::cout << std::endl;

	std::cout << "Port: " << port << std::endl;

	std::cout << "Max bodysize: " << max_body_size << std::endl;

	std::cout << "Error pages: ";
	for (ErrorPage ep : err_pages) {
		for (int err : ep.errs) {
			std::cout << err << " ";
		}
		std::cout << ep.path << std::endl;
	} std::cout << std::endl;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */