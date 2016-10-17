// Identify the UE sCellCandidate based on enbSCellFreqCandidateList and ueCaSupportedBandList
UehCommonCtxtEnbSCellFreqCandidateVectorD ueSCellFreqCandidateList;
const RnhRadioConfConfigurationDataPtrD momDataPtr= ueCtxtDataPtr->getCellDataPtr()->getConfigurationDataPtr();
// PCell--------------------------------------------------------------------------------------------------------------------------------------------
bool isMfbiSupported = caLicenseManagerPtr->isMfbiLicenseEnabled();
RnhNCellEUtranGenericFrequencyD pCellGenericFrequency;
RnhNCellFreqBandUtilD::getSourceCellGenericFrequency(momDataPtr->getservingEUtranGenericCell(), pCellGenericFrequency);
U32 pCellUeEarfcn = RnhNCellFreqBandUtilD::getUeEarfcn(eutranCapabilitiesPtr, &pCellGenericFrequency, isMfbiSupported);
U32 pCellUeFreqBand = eutranCapabilitiesPtr->getEutranOperatingBand(pCellUeEarfcn);
S32 disableCaBwCombinationCheck = 0;
U32 pCellId = ueCtxtDataPtr->getCellDataPtr()->getCellId();
 
// --------------------------------------------------------------------------------------------------------------------------------------------
//first scell
const RnhNCellEUtranCellGenericInfoIfD* firstSCellInfoPtr = NULL;
U32 firstScellUeEarfcn = UEH_INVALID_U32;
U32  firstScellFreqBand = UEH_INVALID_U32;
 
if (excludedSCellFreq != UEH_INVALID_U32)
{
  firstScellUeEarfcn  = excludedSCellFreq;
  firstScellFreqBand = eutranCapabilitiesPtr->getEutranOperatingBand(firstScellUeEarfcn);
  firstSCellInfoPtr =
      ueCtxtDataPtr->getFreqInfo().getLookupByCgiHandler()->getEUtranCellInfo(pCellId,
                                                                              getConfiguredScellCgiFromEarfcn(firstScellUeEarfcn));
 
}
 
UehCommonCtxtEnbSCellFreqCandidateVectorD enbSCellFreqCandidateList =
    ueCtxtDataPtr->getCellDataPtr()->getEnbSCellFreqCandidateList();
 
if(ueCaSupportedBandList.empty())
{
  return ueSCellFreqCandidateList;
}
 
ELIB_TRACE_UE_LEVEL2(
    ueCtxtDataPtr->getUeTraceHandlerPtr(),
    ueCtxtDataPtr->getRacUeRef(),
    Ft_CARRIER_AGGREGATION,
    "calculateUeSCellCandidateListCommon enbSCellFreqCandidateList size =%d",
    enbSCellFreqCandidateList.size());
 
for (UehCommonCtxtEnbSCellFreqCandidateVectorD::iterator freqIter = enbSCellFreqCandidateList.begin();
    freqIter != enbSCellFreqCandidateList.end();
    freqIter++)
{
  /* for each frequency */
  U32 freqId = (*freqIter).freqId;
  UehCommonCtxtScellFreqPriorityInfoE sCellFreqPriority = (*freqIter).sCellFreqPriority;
  UehCommonCtxtEnbSCellCandidateVectorD enbSCellCandidateList = (*freqIter).getSCellCandidateList();
  UehCommonCtxtEnbSCellCandidateVectorD ueSCellCandidateList;
  UehCommonCtxtEnbSCellFreqCandidateD ueSCellFreqCandidate;
  UehCommonCtxtEnbSCellCandidateD ueSCellCandidate;
 
  ueSCellCandidateList.clear();
  // save the all sCells in this freq in the UE candidate list.
  for(UehCommonCtxtEnbSCellCandidateVectorD::iterator cellIter = enbSCellCandidateList.begin();
      cellIter != enbSCellCandidateList.end();
      cellIter++)
  {
    // SCell--------------------------------------------------------------------------------------------------------------------------------------------
    const RnhNCellEUtranCellGenericInfoIfD* sCellInfoPtr =
        ueCtxtDataPtr->getFreqInfo().getLookupByCgiHandler()->getEUtranCellInfo(pCellId, cellIter->cgi);
 
    S32 sCellTxModeConfigureInMom = sCellInfoPtr->getConfiguredTxMode();
    S16 sCellTxMode;
    S32 sCellNoOfUsedTxAntenna =
        ueCtxtDataPtr->getCellDataPtr()->getServingCellHandlerPtr()->getNoOfUsedTxAntennas(cellIter->cgi.eNodeBCellId.cellId);
    // --------------------------------------------------------------------------------------------------------------------------------------------
 
    if (isSCellCandidateValid(sCellInfoPtr) &&
        (excludedSCellFreq != sCellInfoPtr->getGenericFrequency()->getEarfcn()))
    {
      if ((sCellInfoPtr->getIsDlOnly() == 1) ||
          (sCellInfoPtr->getCellBarred() == 0))
      {
        // if dl only cell then dl only license should be valid
        if (!(caLicenseManagerPtr->checkDlSupplemntaryScellLicense()))
        {
          // DL Only license is not operable.
          // moving on to next potential SCell candidate.
          ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon DlOnly service state In-Operable. No SCell CGI=%s",
              cellIter->cgi.toDecimal().c_str());
         
          continue;
        }
      }
 
      // if license service state is INOPERABLE, no TDD SCell if the PCell is FDD.
      if(momDataPtr->getservingEUtranGenericCell()->froType == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
      {
        // FDD PCELL
        if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
        {
          // TDD SCELL
          // FDD TDD CA license.
          if(ueCtxtDataPtr->isLicenseOperable(ELIB_CONST_LICENSE_CARRIER_AGGREGATION_FDD_TDD_ID) == false)
          {
 
            // FDD TDD CA license service state is not OPERABLE.
            // NO TDD SCell when PCell is FDD
            // Move to next potential SCell candidate        
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon FDD TDD CA license service state In-Operable. If PCell is FDD, no TDD SCell CGI=%s",
                cellIter->cgi.toDecimal().c_str());
 
            continue;
          }
 
          // PCell is FDD, SCell is TDD.
          if(!initialFddPcellTddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon FDD PCell, TDD SCell Tx Mode is not supported. TDD SCell CGI=%s",
                cellIter->cgi.toDecimal().c_str());
 
            continue;
          }
        }
        else if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
        {
          // FDD SCELL
          // PCell is FDD, SCell is TDD.
          if(!initialFddPcellFddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
                ueCtxtDataPtr->getUeTraceHandlerPtr(),
                ueCtxtDataPtr->getRacUeRef(),
                Ft_CARRIER_AGGREGATION,
                "calculateUeSCellCandidateListCommon FDD PCell, FDD SCell Tx Mode is not supported. FDD SCell CGI=%s",
                cellIter->cgi.toDecimal().c_str());
 
            continue;
          }
        }
      }
      else if (momDataPtr->getservingEUtranGenericCell()->froType == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
      {
        // for TDD TDD CA, not all the TDD sCellTxModes are supported
        // TDD sCellTxMode check
        // PCell is TDD, SCell is TDD
 
        //TDD PCell TDD SCell,
        //CmccFastTrack WP4913 start
        if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLTDD_FRO_TYPE)
        {
          if(!initialTddPcellTddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon TDD SCell Tx Mode and TDD PCell Tx Mode combination is not supported. TDD SCell CGI=%s",
              cellIter->cgi.toDecimal().c_str());
 
            continue;
          }
        }
        else if (sCellInfoPtr->getCellFroRef().getFroType() == ELIB_CONST_EUTRANCELLFDD_FRO_TYPE)
        {
          if(ueCtxtDataPtr->getSysConstValue(SYS_CONST_CMCC_FT_FDD_TDD_CA_TDD_AS_PCELL_ENABLED_ID) == 0)
          {
            // TDD FDD CA SC is 0.
            ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon TDD-FDD CA is not supported. TDD SCell CGI=%s",
              cellIter->cgi.toDecimal().c_str());
            // NO FDD SCell when PCell is TDD
            continue;
          }
          if(!initialTddPcellFddScellTxModeCalculation(sCellTxMode, sCellTxModeConfigureInMom, sCellNoOfUsedTxAntenna))
          {
            // sCellTxMode is not valid
            // Move to next SCell candidate.
            ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon FDD SCell Tx Mode and TDD PCell Tx Mode combination is not supported. TDD SCell CGI=%s",
              cellIter->cgi.toDecimal().c_str());
 
            continue;
          }
        }
        //CmccFastTrack WP4913 end
      }
 
      RnhRadioConfEUtranGenericCellIfD* pCellEUtranGenericCellPtr = momDataPtr->getservingEUtranGenericCell();
      RnhNCellEUtranGenericFrequencyRangeD pCellGenericFrequencyRange(&pCellGenericFrequency, RnhNCellUtilD::getEutranNdlBandWidth(pCellEUtranGenericCellPtr));
      RnhNCellEUtranGenericFrequencyRangeD sCellGenericFrequencyRange(sCellInfoPtr->getGenericFrequency(), sCellInfoPtr->getChannelBandwidth());
      if ((ueCtxtDataPtr->getSysConstValue(SYS_CONST_DISABLE_CA_BW_COMBINATION_ID) == 1)||
          (eutranCapabilitiesPtr->is3CcSupported() 
              && ueCtxtDataPtr->getSysConstValue(SYS_CONST_ENABLE3C_CA_BW_COMBINATION_ID) == 0))
      {
        disableCaBwCombinationCheck = 1;
      }
      U32 sCellUeFreqBand = RnhNCellFreqBandUtilD::getCarrierAggregationSCellUeFreqBandCommon(eutranCapabilitiesPtr,
                                                                                              &pCellGenericFrequencyRange,
                                                                                              &sCellGenericFrequencyRange,
                                                                                              disableCaBwCombinationCheck,
                                                                                              isMfbiSupported,
                                                                                              pCellUeFreqBand,
                                                                                              ueCaSupportedBandList,
                                                                                              ueCtxtDataPtr->getSysConstValue(SYS_CONST_CMCC_FT_ENABLE_MFBI_CA_ID));
      if ((sCellUeFreqBand != RnhNCellFreqInfoConstantsD::RNH_NCELL_FREQ_INFO_CONSTANTS_BAND_UNDEFINED)
          && (firstSCellInfoPtr != NULL))
      {
        //extra check for  pcell +scell1 + scell2 are needed for 3C ue
        RnhNCellEUtranGenericFrequencyRangeD firstSCellFrequencyRange(firstSCellInfoPtr->getGenericFrequency(),firstSCellInfoPtr->getChannelBandwidth());
        sCellUeFreqBand = RnhNCellFreqBandUtilD::getThirdFreqBandForCarrierAggregation (eutranCapabilitiesPtr,
                                                                                        &pCellGenericFrequencyRange,
                                                                                        &firstSCellFrequencyRange,
                                                                                        &sCellGenericFrequencyRange,
                                                                                        pCellUeEarfcn,
                                                                                        pCellUeFreqBand,
                                                                                        firstScellUeEarfcn,
                                                                                        firstScellFreqBand,
                                                                                        disableCaBwCombinationCheck,
                                                                                        isMfbiSupported);
      }
     
      if (sCellUeFreqBand != RnhNCellFreqInfoConstantsD::RNH_NCELL_FREQ_INFO_CONSTANTS_BAND_UNDEFINED)
      {
        U32 sCellUeEarfcn = RnhNCellFreqBandUtilD::getCarrierAggregationSCellUeEarfcnCommon(eutranCapabilitiesPtr,
                                                                                            &pCellGenericFrequencyRange,
                                                                                            &sCellGenericFrequencyRange,
                                                                                            disableCaBwCombinationCheck,
                                                                                            isMfbiSupported,
                                                                                            ueCaSupportedBandList,
                                                                                            ueCtxtDataPtr->getSysConstValue(SYS_CONST_CMCC_FT_ENABLE_MFBI_CA_ID));
        if((sCellUeFreqBand != pCellUeFreqBand) &&
           (eutranCapabilitiesPtr->isUlBandAggregationSupported(pCellUeFreqBand)))
        {
          /* CA inter Band -----  */       
          ueSCellCandidate.cgi = (*cellIter).cgi;
          ueSCellCandidate.sCellPriority = getSCellCandidatePriority(sCellInfoPtr, pCellId, alg);
 
          if (sCellInfoPtr->getIsDlOnly() != 1)
          {
            // push the SCell candidate in the back
            ueSCellCandidateList.push_back(ueSCellCandidate);
          }
          else
          {
            // isDlOnly Cell add to top of list, select dl only cell first.
            ueSCellCandidateList.insert(ueSCellCandidateList.begin(), ueSCellCandidate);
          }
         
          ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon inter band : CGI=%s freqId=%d sCellFreqPriority=%d sCellPriority=%d",
              ueSCellCandidate.cgi.toDecimal().c_str(),
              freqId,
              sCellFreqPriority,
              ueSCellCandidate.sCellPriority);
        }
        else if ((sCellUeFreqBand == pCellUeFreqBand) &&
            (pCellUeEarfcn != sCellUeEarfcn))
        {
          /* CA intra band */
          ueSCellCandidate.cgi = (*cellIter).cgi;
          ueSCellCandidate.sCellPriority = getSCellCandidatePriority(sCellInfoPtr, pCellId, alg);
 
          if (sCellInfoPtr->getIsDlOnly() != 1)
          {
            // push the SCell candidate in the back
            ueSCellCandidateList.push_back(ueSCellCandidate);
          }
          else
          {
            // isDlOnly Cell add to top of list, select dl only cell first.
            ueSCellCandidateList.insert(ueSCellCandidateList.begin(), ueSCellCandidate);
          }
         
      
          ELIB_TRACE_UE_LEVEL2(
              ueCtxtDataPtr->getUeTraceHandlerPtr(),
              ueCtxtDataPtr->getRacUeRef(),
              Ft_CARRIER_AGGREGATION,
              "calculateUeSCellCandidateListCommon intra band : CGI=%s freqId %d sCellFreqPriority=%d sCellPriority=%d",
              ueSCellCandidate.cgi.toDecimal().c_str(),
              freqId,
              sCellFreqPriority,
              ueSCellCandidate.sCellPriority);
        }
      }
    } // isSCellCandidateValid()
  }
  // prioritzed the SCell candidate list based on DlOnly and CoverageIndicator
  ueSCellCandidateList = calculatePrioritizedUeSCellCandidateList(ueSCellCandidateList, alg);
    
  // WP3714 Filter the candidate list further based on Cross Du license
  ueSCellCandidateList = calculateCrossDuUeSCellCandidateList(ueSCellCandidateList);
 
  if(!ueSCellCandidateList.empty())
  {
    ueSCellFreqCandidate.setSCellCandidateList(ueSCellCandidateList);
    ueSCellFreqCandidate.freqId = freqId;
   
    ueSCellFreqCandidate.sCellFreqPriority = sCellFreqPriority;
   
    //add the Freq, TDD SCell freq is first in the list.
    if (sCellFreqPriority == UEH_COMMON_SCELL_FREQ_PRIORITY_TDD)
    {
      ueSCellFreqCandidateList.insert(ueSCellFreqCandidateList.begin(),ueSCellFreqCandidate);
    }
    else // FDD SCell include DlOnly
    {
      ueSCellFreqCandidateList.push_back(ueSCellFreqCandidate);
    }
  }
}
 
// Prioritize the commonUeSCellFreqCandidateList based on TDD, FDD and DlOnly
return calculatePrioritizedUeSCellFreqCandidateList(ueSCellFreqCandidateList, alg);